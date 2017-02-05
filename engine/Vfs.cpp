#include "Common.h"
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


bool InitVfs( const char* argv0 )
{
    RealMountSystem = InitVfs_Real();
    if(!RealMountSystem)
        return false;

    PhysFSMountSystem = InitVfs_PhysFS(argv0);
    if(!PhysFSMountSystem)
        return false;

    vec_init(&Mounts);
    vec_init(&SearchPaths);
    return true;
}

void DestroyVfs()
{
    // TODO: Forcefully unmount everything and Error() the situation!
    vec_deinit(&Mounts);
    vec_deinit(&SearchPaths);

    RealMountSystem->destroy();
    PhysFSMountSystem->destroy();
}


// --- Mounting ---

static bool IsValidMountPoint( const char* vfsPath )
{
    // The path must have *one* slash at the beginning:
    return strrchr(vfsPath, '/') == vfsPath;
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

/*
bool MountPackage( const char* reference )
{
    // TODO
    return true;
}
*/


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

    if(vfsPath[0] != '/')
        return NULL;

    const char* separator = strchr(&vfsPath[1], '/');
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

    if(!subMountPath)
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
    return file->mountSystem->readFile(file, buffer, size);
}

int WriteVfsFile( VfsFile* file, const void* buffer, int size )
{
    return file->mountSystem->writeFile(file, buffer, size);
}

bool SetVfsFilePos( VfsFile* file, int position )
{
    return file->mountSystem->setFilePos(file, position);
}

int GetVfsFilePos( const VfsFile* file )
{
    return file->mountSystem->getFilePos(file);
}

int GetVfsFileSize( const VfsFile* file )
{
    return file->mountSystem->getFileSize(file);
}

bool HasVfsFileEnded( const VfsFile* file )
{
    return file->mountSystem->hasFileEnded(file);
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
    if(strcmp(vfsPath, "/") == 0)
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
