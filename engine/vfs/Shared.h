#ifndef __KONSTRUKT_VFS_SHARED__
#define __KONSTRUKT_VFS_SHARED__

#include "../FsUtils.h" // MAX_PATH_SIZE
#include "../Vfs.h" // PathList, VfsFileInfo


struct MountSystem;

struct Mount
{
    const MountSystem* mountSystem;
    char vfsPath[MAX_PATH_SIZE];
    char realPath[MAX_PATH_SIZE];
    bool writingAllowed; // only applicable to `REAL_MOUNT`
};

struct MountSystem
{
    void (*destroy)();
    void (*mount)( Mount* mount );
    void (*unmount)( const Mount* mount );
    void* (*openFile)( const Mount* mount,
                       const char* subMountPath,
                       VfsOpenMode mode );
    void (*closeFile)( const void* file );
    int (*readFile)( void* file, void* buffer, int size );
    int (*writeFile)( void* file, const void* buffer, int size );
    void (*setFilePos)( void* file, int position );
    int  (*getFilePos)( const void* file );
    int  (*getFileSize)( const void* file );
    bool (*hasFileEnded)( const void* file );
    PathList* (*getDirEntries)( const Mount* mount, const char* subMountPath );
    VfsFileInfo (*getFileInfo)( const Mount* mount, const char* subMountPath );
    void (*deleteFile)( Mount* mount, const char* subMountPath );
    void (*makeDir)( Mount* mount, const char* subMountPath );
};

#endif
