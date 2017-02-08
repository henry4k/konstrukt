#include <assert.h>

#include "Common.h"
#include "Constants.h"
#include "Vfs.h"
#include "vfs/Shared.h"
#include "vfs/Real.h"
#include "vfs/PhysFS.h"


struct VfsFile
{
    const MountSystem* mountSystem;
    void* handle;
};


MountSystem* RealMountSystem;
MountSystem* PhysFSMountSystem;
vec_t(Mount) Mounts;
vec_t(Path) SearchPaths;
char TempStateDirectory[MAX_PATH_SIZE];
char TempSharedStateDirectory[MAX_PATH_SIZE];


static bool SetWriteDirectory( const char* name,
                               const char* path,
                               char* tempDirOut )
{
    bool isTempDir = false;

    if(!path)
    {
        isTempDir = true;
        path = CreateTemporaryDirectory(name);
        if(!path)
            return false;
    }

    if(!MountVfsDir(name, path, true))
        return false;

    if(isTempDir)
        CopyString(path, tempDirOut, MAX_PATH_SIZE);
    else
        tempDirOut[0] = '\0';

    return true;
}

bool InitVfs( const char* argv0,
              const char* stateDirectory,
              const char* sharedStateDirectory )
{
    RealMountSystem = InitVfs_Real();
    if(!RealMountSystem)
        return false;

    PhysFSMountSystem = InitVfs_PhysFS(argv0);
    if(!PhysFSMountSystem)
        return false;

    vec_init(&Mounts);
    vec_init(&SearchPaths);

    if(!SetWriteDirectory("state",
                          stateDirectory,
                          TempStateDirectory))
        return false;

    if(!SetWriteDirectory("shared-state",
                          sharedStateDirectory,
                          TempSharedStateDirectory))
        return false;

    AddPackageSearchPath(DEFAULT_PACKAGE_SEARCH_PATH);

    return true;
}

void DestroyVfs()
{
    int i;
    Mount* mount;
    vec_foreach_ptr(&Mounts, mount, i)
        mount->mountSystem->unmount(mount);

    if(TempStateDirectory[0] != '\0')
        RemoveDirectoryTree(TempStateDirectory);
    if(TempSharedStateDirectory[0] != '\0')
        RemoveDirectoryTree(TempSharedStateDirectory);

    vec_deinit(&Mounts);
    vec_deinit(&SearchPaths);

    RealMountSystem->destroy();
    PhysFSMountSystem->destroy();
}


// --- Mounting ---

static bool IsValidMountPoint( const char* vfsPath )
{
    // There may be no directory separators:
    return strchr(vfsPath, '/') == NULL;
}

static Mount* FindMountByVfsPath( const char* vfsPath, int* indexOut )
{
    int i;
    Mount* mount;
    vec_foreach_ptr(&Mounts, mount, i)
    {
        if(strcmp(mount->vfsPath, vfsPath) == 0)
        {
            if(indexOut)
                *indexOut = i;
            return mount;
        }
    }
    return NULL;
}

bool MountVfsDir( const char* vfsPath,
                  const char* realPath,
                  bool writingAllowed )
{
    if(!IsValidMountPoint(vfsPath))
    {
        Error("'%s' is not a valid mount point.", vfsPath);
        return NULL;
    }

    if(FindMountByVfsPath(vfsPath, NULL))
    {
        Error("'%s' is already mounted to something.", vfsPath);
        return NULL;
    }

    const MountSystem* mountSystem;
    if(writingAllowed)
        mountSystem = RealMountSystem;
    else
        mountSystem = PhysFSMountSystem;

    Mount* mount = vec_push_ptr(&Mounts);
    memset(mount, 0, sizeof(Mount));
    mount->mountSystem = mountSystem;
    CopyString(vfsPath,  mount->vfsPath,  MAX_PATH_SIZE);
    CopyString(realPath, mount->realPath, MAX_PATH_SIZE);
    mount->writingAllowed = writingAllowed;

    if(!mountSystem->mount(mount))
    {
        Mounts.length--;
        return NULL;
    }

    Log("Mounted '%s' to '%s'.", realPath, vfsPath);

    return mount;
}

void UnmountVfsDir( const char* vfsPath )
{
    int mountIndex;
    Mount* mount = FindMountByVfsPath(vfsPath, &mountIndex);
    if(!mount)
    {
        Error("Mount point '%s' does not exist.", vfsPath);
        return;
    }

    mount->mountSystem->unmount(mount);

    vec_splice(&Mounts, mountIndex, 1);
}


// --- Package mount utilities ---

void AddPackageSearchPath( const char* path )
{
    Path* searchPath = vec_push_ptr(&SearchPaths);
    CopyString(path, searchPath->str, sizeof(Path));
}

static const char* ResolvePackageIdWithBasePath( const char* id,
                                                 const char* basePath )
{
    static char path[MAX_PATH_SIZE];

    FormatBuffer(path, sizeof(path), "%s%c%s", basePath, NATIVE_DIR_SEP, id);
    if(GetFileType(path) == FILE_TYPE_DIRECTORY)
        return path;

    FormatBuffer(path, sizeof(path), "%s%c%s.zip", basePath, NATIVE_DIR_SEP, id);
    if(GetFileType(path) == FILE_TYPE_DIRECTORY)
        return path;

    return NULL;
}

static const char* ResolvePackageIdWithSearchPath( const char* id )
{
    int i;
    Path* basePath;
    vec_foreach_ptr(&SearchPaths, basePath, i)
    {
        const char* path = ResolvePackageIdWithBasePath(id, basePath->str);
        if(path)
            return path;
    }
    return NULL;
}

static const char* ResolvePackageReference( const char* reference )
{
    const FileType type = GetFileType(reference);
    if(type == FILE_TYPE_DIRECTORY ||
       type == FILE_TYPE_REGULAR)
        return reference;
    else
        return ResolvePackageIdWithSearchPath(reference);
}

static const char* GetBaseName( const char* path )
{
    int i = strlen(path)-1;
    for(; i >= 0; i--)
        if(path[i] == '/' ||
           path[i] == '\\')
            return &path[i+1];
    return path;
}

static const char* ExtractPackageNameFromReference( const char* reference )
{
    static char buffer[MAX_PATH_SIZE];

    const char* baseName = GetBaseName(reference);

    int i = 0;
    for(;;i++)
        if(baseName[i] == '.' || baseName[i] == '\0')
            break;

    assert(i >= 0);
    assert(i < MAX_PATH_SIZE-1);
    memcpy(buffer, baseName, i);
    buffer[i] = '\0';

    return buffer;
}

bool MountPackage( const char* reference )
{
    const char* packagePath = ResolvePackageReference(reference);
    if(packagePath)
    {
        const char* name = ExtractPackageNameFromReference(reference);
        if(MountVfsDir(name, packagePath, false))
        {
            Log("Mounted package '%s' (%s).", name, packagePath);
            return true;
        }
        else
        {
            Error("Can't mount package '%s' (%s):", name, packagePath);
            return false;
        }
    }
    else
    {
        Error("Can't find package '%s'.", reference);
        return false;
    }
}


// --- File access ---

/**
 * @param subMountPath
 * The sub mount path. (Which is the rear part of `vfsPath`.)
 *
 * @return
 * Returns the mount point, which starts at the beginning of `vfsPath`.
 */
static const char* SplitVfsPath( const char* vfsPath,
                                 const char** subMountPath )
{
    static char mountPointBuffer[MAX_PATH_SIZE];

    const char* separator = strchr(vfsPath, '/');
    if(separator)
    {
        const size_t pos = separator - vfsPath;
        if(pos >= MAX_PATH_SIZE)
            return NULL;
        CopyString(vfsPath, mountPointBuffer, pos+1);
        *subMountPath = separator+1; // sub mount path starts behind first separator
    }
    else
    {
        CopyString(vfsPath, mountPointBuffer, MAX_PATH_SIZE);
        *subMountPath = NULL;
    }
    return mountPointBuffer;
}

VfsFile* OpenVfsFile( const char* vfsPath, VfsOpenMode mode )
{
    const char* subMountPath;
    const char* mountPoint = SplitVfsPath(vfsPath, &subMountPath);

    if(!mountPoint || !subMountPath)
    {
        Error("Can't open file '%s'.", vfsPath);
        return NULL;
    }

    const Mount* mount = FindMountByVfsPath(mountPoint, NULL);
    if(!mount)
    {
        Error("Can't open file '%s' as mount point does not exist.", vfsPath);
        return NULL;
    }

    void* handle = mount->mountSystem->openFile(mount, subMountPath, mode);
    if(!handle)
        return NULL;

    VfsFile* file = NEW(VfsFile);
    file->mountSystem = mount->mountSystem;
    file->handle = handle;
    return file;
}

void CloseVfsFile( VfsFile* file )
{
    file->mountSystem->closeFile(file);
    DELETE(VfsFile, file);
}

int ReadVfsFile( VfsFile* file, void* buffer, int size )
{
    return file->mountSystem->readFile(file->handle, buffer, size);
}

int WriteVfsFile( VfsFile* file, const void* buffer, int size )
{
    return file->mountSystem->writeFile(file->handle, buffer, size);
}

bool SetVfsFilePos( VfsFile* file, int position )
{
    return file->mountSystem->setFilePos(file->handle, position);
}

int GetVfsFilePos( const VfsFile* file )
{
    return file->mountSystem->getFilePos(file->handle);
}

int GetVfsFileSize( const VfsFile* file )
{
    return file->mountSystem->getFileSize(file->handle);
}

bool HasVfsFileEnded( const VfsFile* file )
{
    return file->mountSystem->hasFileEnded(file->handle);
}


// --- File system access ---

static PathList* GetMountNames()
{
    PathList* list = NEW(PathList);
    int i;
    Mount* mount;
    vec_foreach_ptr(&Mounts, mount, i)
    {
        const char* entryName = mount->vfsPath+1; // Skip the initial slash
        Path* entry = vec_push_ptr(list);
        CopyString(entryName, entry->str, sizeof(Path));
    }
    return list;
}

PathList* GetVfsDirEntries( const char* vfsPath )
{
    if(vfsPath[0] == '\0')
    {
        return GetMountNames();
    }
    else
    {
        const char* subMountPath;
        const char* mountPoint = SplitVfsPath(vfsPath, &subMountPath);
        const Mount* mount = FindMountByVfsPath(mountPoint, NULL);
        if(!mount)
        {
            Error("Mount point '%s' does not exist.", mountPoint);
            return NULL;
        }

        return mount->mountSystem->getDirEntries(mount, subMountPath);
    }
}

static VfsFileInfo GetInvalidFileInfo()
{
    VfsFileInfo info;
    memset(&info, 0, sizeof(info));
    return info;
}

VfsFileInfo GetVfsFileInfo( const char* vfsPath )
{
    const char* subMountPath;
    const char* mountPoint = SplitVfsPath(vfsPath, &subMountPath);
    const Mount* mount = FindMountByVfsPath(mountPoint, NULL);
    if(!mount)
    {
        Error("Mount point '%s' does not exist.", mountPoint);
        return GetInvalidFileInfo();
    }

    return mount->mountSystem->getFileInfo(mount, subMountPath);
}

bool DeleteVfsFile( const char* vfsPath )
{
    const char* subMountPath;
    const char* mountPoint = SplitVfsPath(vfsPath, &subMountPath);
    Mount* mount = FindMountByVfsPath(mountPoint, NULL);
    if(!mount)
    {
        Error("Mount point '%s' does not exist.", mountPoint);
        return false;
    }

    return mount->mountSystem->deleteFile(mount, subMountPath);
}

bool MakeVfsDir( const char* vfsPath )
{
    const char* subMountPath;
    const char* mountPoint = SplitVfsPath(vfsPath, &subMountPath);
    Mount* mount = FindMountByVfsPath(mountPoint, NULL);
    if(!mount)
    {
        Error("Mount point '%s' does not exist.", mountPoint);
        return false;
    }

    return mount->mountSystem->makeDir(mount, subMountPath);
}
