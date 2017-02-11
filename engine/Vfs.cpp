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
ArrayList(Mount) Mounts;
ArrayList(Path) SearchPaths;
char TempStateDirectory[MAX_PATH_SIZE];
char TempSharedStateDirectory[MAX_PATH_SIZE];


static void SetWriteDirectory( const char* name,
                               const char* path,
                               char* tempDirOut )
{
    bool isTempDir = false;

    if(!path)
    {
        isTempDir = true;
        path = CreateTemporaryDirectory(name);
    }

    MountVfsDir(name, path, true);

    if(isTempDir)
        CopyString(path, tempDirOut, MAX_PATH_SIZE);
    else
        tempDirOut[0] = '\0';
}

void InitVfs( const char* argv0,
              const char* stateDirectory,
              const char* sharedStateDirectory )
{
    RealMountSystem = InitVfs_Real();
    PhysFSMountSystem = InitVfs_PhysFS(argv0);

    InitArrayList(&Mounts);
    InitArrayList(&SearchPaths);

    SetWriteDirectory("state", stateDirectory, TempStateDirectory);
    SetWriteDirectory("shared-state", sharedStateDirectory, TempSharedStateDirectory);

    AddPackageSearchPath(DEFAULT_PACKAGE_SEARCH_PATH);
}

void DestroyVfs()
{
    REPEAT(Mounts.length, i)
    {
        Mount* mount = Mounts.data + i;
        mount->mountSystem->unmount(mount);
    }

    if(TempStateDirectory[0] != '\0')
        RemoveDirectoryTree(TempStateDirectory);
    if(TempSharedStateDirectory[0] != '\0')
        RemoveDirectoryTree(TempSharedStateDirectory);

    DestroyArrayList(&Mounts);
    DestroyArrayList(&SearchPaths);

    RealMountSystem->destroy();
    PhysFSMountSystem->destroy();
}


// --- Mounting ---

static bool IsValidMountPoint( const char* vfsPath )
{
    // There may be no directory separators:
    return strchr(vfsPath, '/') == NULL;
}

static Mount* TryGetMountByVfsPath( const char* vfsPath, int* indexOut )
{
    REPEAT(Mounts.length, i)
    {
        Mount* mount = Mounts.data + i;
        if(strcmp(mount->vfsPath, vfsPath) == 0)
        {
            if(indexOut)
                *indexOut = i;
            return mount;
        }
    }
    return NULL;
}

static Mount* GetMountByVfsPath( const char* vfsPath, int* indexOut )
{
    Mount* mount = TryGetMountByVfsPath(vfsPath, indexOut);
    if(!mount)
        FatalError("Mount point '%s' does not exist.", vfsPath);
    return mount;
}

void MountVfsDir( const char* vfsPath,
                  const char* realPath,
                  bool writingAllowed )
{
    if(!IsValidMountPoint(vfsPath))
        FatalError("'%s' is not a valid mount point.", vfsPath);

    if(TryGetMountByVfsPath(vfsPath, NULL))
        FatalError("'%s' is already mounted to something.", vfsPath);

    const MountSystem* mountSystem;
    if(writingAllowed)
        mountSystem = RealMountSystem;
    else
        mountSystem = PhysFSMountSystem;

    Mount* mount = AllocateAtEndOfArrayList(&Mounts, 1);
    memset(mount, 0, sizeof(Mount));
    mount->mountSystem = mountSystem;
    CopyString(vfsPath,  mount->vfsPath,  MAX_PATH_SIZE);
    CopyString(realPath, mount->realPath, MAX_PATH_SIZE);
    mount->writingAllowed = writingAllowed;

    mountSystem->mount(mount);
    Log("Mounted '%s' to '%s'.", realPath, vfsPath);
}

void UnmountVfsDir( const char* vfsPath )
{
    int mountIndex;
    Mount* mount = GetMountByVfsPath(vfsPath, &mountIndex);
    mount->mountSystem->unmount(mount);
    Log("Unmounted '%s' from '%s'.", mount->realPath, mount->vfsPath);
    RemoveFromArrayList(&Mounts, mountIndex, 1);
}


// --- Package mount utilities ---

void AddPackageSearchPath( const char* path )
{
    Path* searchPath = AllocateAtEndOfArrayList(&SearchPaths, 1);
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
    REPEAT(SearchPaths.length, i)
    {
        const Path* basePath = SearchPaths.data + i;
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

    const char* path = ResolvePackageIdWithSearchPath(reference);
    if(path)
        return path;

    FatalError("Can't resolve package reference '%s'.", reference);
    return NULL;
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

void MountPackage( const char* reference )
{
    const char* packagePath = ResolvePackageReference(reference);
    const char* name = ExtractPackageNameFromReference(reference);
    MountVfsDir(name, packagePath, false);
    Log("Mounted package '%s' (%s).", name, packagePath);
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
        FatalError("Can't open file '%s'.", vfsPath);

    const Mount* mount = GetMountByVfsPath(mountPoint, NULL);
    void* handle = mount->mountSystem->openFile(mount, subMountPath, mode);

    VfsFile* file = NEW(VfsFile);
    file->mountSystem = mount->mountSystem;
    file->handle = handle;
    return file;
}

void CloseVfsFile( VfsFile* file )
{
    file->mountSystem->closeFile(file->handle);
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

void SetVfsFilePos( VfsFile* file, int position )
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
    REPEAT(Mounts.length, i)
    {
        const Mount* mount = Mounts.data + i;
        const char* entryName = mount->vfsPath;
        Path* entry = AllocateAtEndOfArrayList(list, 1);
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
        const Mount* mount = GetMountByVfsPath(mountPoint, NULL);
        return mount->mountSystem->getDirEntries(mount, subMountPath);
    }
}

VfsFileInfo GetVfsFileInfo( const char* vfsPath )
{
    const char* subMountPath;
    const char* mountPoint = SplitVfsPath(vfsPath, &subMountPath);
    const Mount* mount = GetMountByVfsPath(mountPoint, NULL);
    return mount->mountSystem->getFileInfo(mount, subMountPath);
}

void DeleteVfsFile( const char* vfsPath )
{
    const char* subMountPath;
    const char* mountPoint = SplitVfsPath(vfsPath, &subMountPath);
    Mount* mount = GetMountByVfsPath(mountPoint, NULL);
    return mount->mountSystem->deleteFile(mount, subMountPath);
}

void MakeVfsDir( const char* vfsPath )
{
    const char* subMountPath;
    const char* mountPoint = SplitVfsPath(vfsPath, &subMountPath);
    Mount* mount = GetMountByVfsPath(mountPoint, NULL);
    return mount->mountSystem->makeDir(mount, subMountPath);
}
