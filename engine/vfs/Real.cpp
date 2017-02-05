#include <stdio.h> // fopen
#include <string.h> // strcmp, strchr, strrchr, strerror
#include <errno.h>

#include "../Common.h"
#include "../FsUtils.h"
#include "../Vfs.h"
#include "Shared.h"


static void DestroyVfs_Real()
{
    // Nothing to do here.
}

static bool MountVfsDir_Real( Mount* mount )
{
    if(GetFileType(mount->realPath) != FILE_TYPE_DIRECTORY)
    {
        Error("Can't mount '%s': '%s' is not a directory.",
              mount->vfsPath,
              mount->realPath);
        return false;
    }
    return true;
}

static void UnmountVfsDir_Real( const Mount* mount )
{
    // Nothing to do here.
}

static const char* TranslatePathSeparators( const char* path,
                                            char oldSeparator,
                                            char newSeparator )
{
    if(oldSeparator == newSeparator)
        return path;

    static char result[MAX_PATH_SIZE];
    for(int i = 0; i < MAX_PATH_SIZE; i++)
    {
        char c = path[i];
        if(c == oldSeparator)
            c = newSeparator;
        result[i] = c;
        if(c == '\0')
            break;
    }
    return result;
}

static const char* VfsOpenModeToString( VfsOpenMode mode )
{
    switch(mode)
    {
#if defined(_WIN32)
        case VFS_OPEN_READ:   return "rb";
        case VFS_OPEN_WRITE:  return "wb";
        case VFS_OPEN_APPEND: return "ab";
#else
        case VFS_OPEN_READ:   return "r";
        case VFS_OPEN_WRITE:  return "w";
        case VFS_OPEN_APPEND: return "a";
#endif
    }
    FatalError("Unknown VfsOpenMode %d.", mode);
    return NULL;
}

static const char* GetRealPath( const Mount* mount,
                                const char* subMountPath )
{
    if(subMountPath)
    {
        return Format("%s%c%s",
                      mount->realPath,
                      NATIVE_DIR_SEP,
                      TranslatePathSeparators(subMountPath,
                                              '/',
                                              NATIVE_DIR_SEP));
    }
    else
    {
        return mount->realPath;
    }
}

static void* OpenVfsFile_Real( const Mount* mount,
                               const char* subMountPath,
                               VfsOpenMode mode )
{
    if(!subMountPath)
    {
        Error("Can't open file '%s'.", mount->vfsPath);
        return NULL;
    }

    const char* path = GetRealPath(mount, subMountPath);
    FILE* file = fopen(path, VfsOpenModeToString(mode));
    if(!file)
    {
        Error("Can't open file '%s/%s': %s",
              mount->vfsPath,
              subMountPath,
              strerror(errno));
        return NULL;
    }

    return (void*)file;
}

static void CloseVfsFile_Real( const void* file )
{
    if(fclose((FILE*)file) != 0)
        Error("Can't close file: %s", strerror(errno));
}

static int ReadVfsFile_Real( void* file, void* buffer, int size )
{
    return (int)fread(buffer, 1, size, (FILE*)file);
}

static int WriteVfsFile_Real( void* file, const void* buffer, int size )
{
    return (int)fwrite(buffer, 1, size, (FILE*)file);
}

static bool SetVfsFilePos_Real( void* file, int position )
{
    if(fseek((FILE*)file, position, SEEK_SET) == 0)
    {
        return true;
    }
    else
    {
        Error("Can't set file position: %s", strerror(errno));
        return false;
    }
}

static int GetVfsFilePos_Real( const void* file )
{
    int pos = ftell((FILE*)file);
    if(pos >= 0)
    {
        return true;
    }
    else
    {
        Error("Can't get file position: %s", strerror(errno));
        return false;
    }
}

static int GetVfsFileSize_Real( const void* file )
{
    FILE* f = (FILE*)file;
    const int oldPos = ftell(f);
    fseek(f, 0, SEEK_END);
    const int size = ftell(f);
    fseek(f, oldPos, SEEK_SET);
    return size;
}

static bool HasVfsFileEnded_Real( const void* file )
{
    return feof((FILE*)file);
}

static PathList* GetVfsDirEntries_Real( const Mount* mount, const char* subMountPath )
{
    const char* path = GetRealPath(mount, subMountPath);
    return GetDirEntries(path);
}

static VfsFileInfo GetVfsFileInfo_Real( const Mount* mount, const char* subMountPath )
{
    const char* path = GetRealPath(mount, subMountPath);
    VfsFileInfo info;
    info.type = GetFileType(path);
    return info;
}

static bool DeleteVfsFile_Real( Mount* mount, const char* subMountPath )
{
    const char* path = GetRealPath(mount, subMountPath);
    return RemoveFile(path);
}

static bool MakeVfsDir_Real( Mount* mount, const char* subMountPath )
{
    const char* path = GetRealPath(mount, subMountPath);
    return CreateDirectory(path);
}

MountSystem* InitVfs_Real()
{
    static MountSystem sys;
    sys.destroy       = DestroyVfs_Real;
    sys.mount         = MountVfsDir_Real;
    sys.unmount       = UnmountVfsDir_Real;
    sys.openFile      = OpenVfsFile_Real;
    sys.closeFile     = CloseVfsFile_Real;
    sys.readFile      = ReadVfsFile_Real;
    sys.writeFile     = WriteVfsFile_Real;
    sys.setFilePos    = SetVfsFilePos_Real;
    sys.getFilePos    = GetVfsFilePos_Real;
    sys.getFileSize   = GetVfsFileSize_Real;
    sys.hasFileEnded  = HasVfsFileEnded_Real;
    sys.getDirEntries = GetVfsDirEntries_Real;
    sys.getFileInfo   = GetVfsFileInfo_Real;
    sys.deleteFile    = DeleteVfsFile_Real;
    sys.makeDir       = MakeVfsDir_Real;
    return &sys;
}
