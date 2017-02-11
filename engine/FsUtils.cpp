#if defined(_WIN32)
    #define WIN32_LEAN_AND_MEAN
    #define NOGDI
    #include <windows.h>
    #define _CRT_RAND_S
    #include <stdlib.h> // rand_s
    #undef CreateDirectory
#else
    #include <sys/stat.h> // stat, mkdir
    #include <dirent.h> // opendir, readdir, closedir
#endif

#include <stdio.h> // remove
#include <string.h> // strerror
#include <errno.h>

#include "Common.h"
#include "FsUtils.h"


#if defined(_WIN32)
FileType GetFileType( const char* path )
{
    DWORD type = GetFileAttributesA(path);
    if(type == INVALID_FILE_ATTRIBUTES)
        return FILE_TYPE_INVALID;

    if(type & FILE_ATTRIBUTE_DIRECTORY)
        return FILE_TYPE_DIRECTORY;
    else
        return FILE_TYPE_REGULAR;
}
#else
FileType GetFileType( const char* path )
{
    struct stat info;
    if(stat(path, &info) == -1)
        return FILE_TYPE_INVALID;
    if(info.st_mode & S_IFREG)
        return FILE_TYPE_REGULAR;
    else if(info.st_mode & S_IFDIR)
        return FILE_TYPE_DIRECTORY;
    else
        return FILE_TYPE_UNKNOWN;
}
#endif

#if defined(_WIN32)
const char* GetLastErrorAsString()
{
    static char buffer[256];
    FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM,
                   NULL,
                   GetLastError(),
                   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                   buffer, 256,
                   NULL);
    return buffer;
}
#endif

void CreateDirectory( const char* path )
{
#if defined(_WIN32)
    if(!CreateDirectoryA(path, NULL))
    {
        FatalError("Can't create directory '%s': %s",
                   path, GetLastErrorAsString());
    }
#else
    const mode_t mode = S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH; // 0666
    if(mkdir(path, mode) != 0)
    {
        FatalError("Can't create directory '%s': %s",
                   path, strerror(errno));
    }
#endif
}

const char* CreateTemporaryDirectory( const char* name )
{
#if defined(_WIN32)
    char tempPath[MAX_PATH_SIZE];
    if(!GetTempPathA(sizeof(tempPath), tempPath))
    {
        FatalError("Can't create temporary directory '%s': %s",
                   name, GetLastErrorAsString());
    }

    const char* path;
    do
    {
        unsigned int randomValue;
        rand_s(&randomValue);
        path = Format("%s\\konstrukt-%s-%d",
                      tempPath,
                      name,
                      randomValue);
    } while(GetFileType(path) != FILE_TYPE_INVALID);

    CreateDirectory(path);
    return path;
#else
    static char path[MAX_PATH_SIZE];
    CopyString(Format("/tmp/konstrukt-%s-XXXXXX", name), path, sizeof(path));
    if(!mkdtemp(path))
    {
        FatalError("Can't create temporary directory '%s': %s",
                   name, strerror(errno));
    }
    return path;
#endif
}

static bool IsValidEntry( const char* entry )
{
    return (strcmp(entry, ".") != 0) &&
           (strcmp(entry, "..") != 0);
}

PathList* GetDirEntries( const char* path )
{
    PathList* list = NEW(PathList);
    InitArrayList(list);
#if defined(_WIN32)
    // Add "\\*" to path:
    char buffer[MAX_PATH_SIZE];
    CopyString(path, buffer, sizeof(searchPath));
    path = Format("%s\\*", buffer);

    WIN32_FIND_DATA dirEntry;
    HANDLE dir = FindFirstFile(path, &dirEntry);
    if(dir == INVALID_HANDLE_VALUE)
        FatalError("Can't read directory '%s'.", path);

    do
    {
        if(IsValidEntry(dirEntry.cFileName))
        {
            Path* entry = AllocateAtEndOfArrayList(list, 1);
            CopyString(dirEntry.cFileName, entry->str, sizeof(Path));
        }
    } while(FindNextFile(dir, &dirEntry));

    FindClose(dir);
#else
    DIR* dir = opendir(path);
    if(!dir)
        FatalError("Can't read directory '%s': %s", path, strerror(errno));

    for(;;)
    {
        struct dirent* dirEntry = readdir(dir);
        if(dirEntry)
        {
            if(IsValidEntry(dirEntry->d_name))
            {
                Path* entry = AllocateAtEndOfArrayList(list, 1);
                CopyString(dirEntry->d_name, entry->str, sizeof(Path));
            }
        }
        else
            break;
    }

    closedir(dir);
#endif
    return list;
}

void FreePathList( PathList* list )
{
    DestroyArrayList(list);
    DELETE(PathList, list);
}

void RemoveFile( const char* path )
{
    if(remove(path) != 0)
        FatalError("Can't remove '%s': %s", path, strerror(errno));
}

void RemoveDirectoryTree( const char* path )
{
    switch(GetFileType(path))
    {
        case FILE_TYPE_REGULAR:
            return RemoveFile(path);

        case FILE_TYPE_DIRECTORY:
        {
            PathList* entries = GetDirEntries(path);
            REPEAT(entries->length, i)
            {
                const Path* entry = entries->data + i;
                char entryPath[MAX_PATH_SIZE];
                FormatBuffer(entryPath, MAX_PATH_SIZE, "%s%c%s",
                        path, NATIVE_DIR_SEP, entry->str);
                RemoveDirectoryTree(entryPath);
            }
            FreePathList(entries);
            return RemoveFile(path);
        }

        case FILE_TYPE_UNKNOWN:
            FatalError("Encountered a file of unknown type: %s", path);

        case FILE_TYPE_INVALID:
            FatalError("Can't delete invalid files: %s", path);
    }
}
