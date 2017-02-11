#include <physfs.h>

#include "../Common.h"
#include "../Vfs.h"
#include "Shared.h"


// PhysFS backwards compatibility:
#if PHYSFS_VER_MAJOR < 2 || (PHYSFS_VER_MAJOR == 2 && PHYSFS_VER_MINOR == 0)
#define PHYSFS_NO_STAT
#define PHYSFS_readBytes( handle, buffer, size ) PHYSFS_read((handle), (buffer), 1, (size))
#define PHYSFS_unmount PHYSFS_removeFromSearchPath
#endif


static void DestroyVfs_PhysFS()
{
    if(!PHYSFS_deinit())
        FatalError("PHYSFS_deinit: %s", PHYSFS_getLastError());
}

static void MountVfsDir_PhysFS( Mount* mount )
{
    if(!PHYSFS_mount(mount->realPath, mount->vfsPath, true))
        FatalError("Can't mount '%s': %s", mount->vfsPath, PHYSFS_getLastError());
}

static void UnmountVfsDir_PhysFS( const Mount* mount )
{
    if(!PHYSFS_unmount(mount->realPath))
        FatalError("Can't unmount '%s': %s", mount->vfsPath, PHYSFS_getLastError());
}

static void* OpenVfsFile_PhysFS( const Mount* mount,
                                 const char* subMountPath,
                                 VfsOpenMode mode )
{
    if(!subMountPath)
        FatalError("Can't open file: '%s'", mount->vfsPath);

    if(mode != VFS_OPEN_READ)
        FatalError("Can't write to files mounted as read-only.");

    const char* path = Format("%s/%s", mount->vfsPath, subMountPath);
    PHYSFS_File* file = PHYSFS_openRead(path);
    if(!file)
        FatalError("PHYSFS_openRead: %s", PHYSFS_getLastError());

    return (void*)file;
}

static void CloseVfsFile_PhysFS( const void* file )
{
    if(!PHYSFS_close((PHYSFS_File*)file))
        FatalError("PHYSFS_close: %s", PHYSFS_getLastError());
}

static int ReadVfsFile_PhysFS( void* file, void* buffer, int size )
{
    return (int)PHYSFS_readBytes((PHYSFS_File*)file, buffer, size);
}

static int WriteVfsFile_PhysFS( void* file, const void* buffer, int size )
{
    FatalError("Mount is read only.");
    return 0;
}

static void SetVfsFilePos_PhysFS( void* file, int position )
{
    if(!PHYSFS_seek((PHYSFS_File*)file, position))
        FatalError("PHYSFS_seek: %s", PHYSFS_getLastError());
}

static int GetVfsFilePos_PhysFS( const void* file )
{
    const int pos = PHYSFS_tell((PHYSFS_File*)file);
    if(pos < 0)
        FatalError("PHYSFS_tell: %s", PHYSFS_getLastError());
    return pos;
}

static int GetVfsFileSize_PhysFS( const void* file )
{
    const int length = PHYSFS_fileLength((PHYSFS_File*)file);
    if(length < 0)
        FatalError("PHYSFS_fileLength: %s", PHYSFS_getLastError());
    return length;
}

static bool HasVfsFileEnded_PhysFS( const void* file )
{
    return PHYSFS_eof((PHYSFS_File*)file);
}

static PathList* GetVfsDirEntries_PhysFS( const Mount* mount, const char* subMountPath )
{
    const char* path;
    if(subMountPath)
        path = Format("%s/%s", mount->vfsPath, subMountPath);
    else
        path = mount->vfsPath;

    PathList* list = NEW(PathList);
    InitArrayList(list);

    char** fileList = PHYSFS_enumerateFiles(path);
    for(int i = 0; fileList[i] != NULL; i++)
    {
        Path* entry = AllocateAtEndOfArrayList(list, 1);
        CopyString(fileList[i], entry->str, sizeof(Path));
    }
    PHYSFS_freeList(fileList);

    return list;
}

static VfsFileInfo GetVfsFileInfo_PhysFS( const Mount* mount, const char* subMountPath )
{
    const char* path;
    if(subMountPath)
        path = Format("%s/%s", mount->vfsPath, subMountPath);
    else
        path = mount->vfsPath;

    VfsFileInfo info;

    if(PHYSFS_exists(path))
    {
        if(PHYSFS_isDirectory(path))
            info.type = FILE_TYPE_DIRECTORY;
        else
            info.type = FILE_TYPE_REGULAR;
    }
    else
        info.type = FILE_TYPE_INVALID;

    return info;
}

static void DeleteVfsFile_PhysFS( Mount* mount, const char* subMountPath )
{
    FatalError("Mount is read only.");
}

static void MakeVfsDir_PhysFS( Mount* mount, const char* subMountPath )
{
    FatalError("Mount is read only.");
}

MountSystem* InitVfs_PhysFS( const char* argv0 )
{
    PHYSFS_Version compiled;
    PHYSFS_VERSION(&compiled);
    Log("Using PhysFS %d.%d.%d", compiled.major, compiled.minor, compiled.patch);

    PHYSFS_Version linked;
    PHYSFS_getLinkedVersion(&linked);
    Log("Linked against PhysFS %d.%d.%d", linked.major, linked.minor, linked.patch);

    if(!PHYSFS_init(argv0))
        FatalError("PHYSFS_init: %s", PHYSFS_getLastError());

    PHYSFS_permitSymbolicLinks(true);

    static MountSystem sys;
    sys.destroy       = DestroyVfs_PhysFS;
    sys.mount         = MountVfsDir_PhysFS;
    sys.unmount       = UnmountVfsDir_PhysFS;
    sys.openFile      = OpenVfsFile_PhysFS;
    sys.closeFile     = CloseVfsFile_PhysFS;
    sys.readFile      = ReadVfsFile_PhysFS;
    sys.writeFile     = WriteVfsFile_PhysFS;
    sys.setFilePos    = SetVfsFilePos_PhysFS;
    sys.getFilePos    = GetVfsFilePos_PhysFS;
    sys.getFileSize   = GetVfsFileSize_PhysFS;
    sys.hasFileEnded  = HasVfsFileEnded_PhysFS;
    sys.getDirEntries = GetVfsDirEntries_PhysFS;
    sys.getFileInfo   = GetVfsFileInfo_PhysFS;
    sys.deleteFile    = DeleteVfsFile_PhysFS;
    sys.makeDir       = MakeVfsDir_PhysFS;
    return &sys;
}
