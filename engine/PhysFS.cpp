#include <string.h> // strlen, memset, memcpy
#include <physfs.h>
#include "Common.h" // CopyString
#include "Game.h" // DEFAULT_PACKAGE_SEARCH_PATHS
#include "PhysFS.h"


static const int MAX_PATH_LENGTH = 255;
static const int MAX_PATH_LIST_LENGTH = 511;
static const char PATH_SEPARATOR = ';';


static char UserDataDirectory[MAX_PATH_LENGTH+1];
static char SearchPaths[MAX_PATH_LIST_LENGTH+1];


static void LogPhysFSVersion()
{
    PHYSFS_Version compiled;
    PHYSFS_Version linked;

    PHYSFS_VERSION(&compiled);
    PHYSFS_getLinkedVersion(&linked);

    Log("Using PhysFS %d.%d.%d", compiled.major, compiled.minor, compiled.patch);
    Log("Linked against PhysFS %d.%d.%d", linked.major, linked.minor, linked.patch);
}

static const char* ResolvePackageNameWithSearchPath( const char* name, const char* searchPath );

bool InitPhysFS( const int argc, char const * const * argv )
{
    memset(&UserDataDirectory, 0, sizeof(UserDataDirectory));
    memset(SearchPaths,        0, sizeof(SearchPaths));

    LogPhysFSVersion();
    if(PHYSFS_init(argv[0]) &&
       PHYSFS_setSaneConfig("apoapsis", // Organization
                            "apoapsis", // Program name
                            NULL, // Extension of automatically mounted archives
                            0, // Include CD-ROMs
                            0)) // Prepend automatically found archives
    {
        assert(PHYSFS_getWriteDir() != NULL);
        CopyString(PHYSFS_getWriteDir(), UserDataDirectory, sizeof(UserDataDirectory));
        const int pathLength = strlen(UserDataDirectory);
        UserDataDirectory[pathLength-1] = '\0'; // Clip the trailing directory separator.
    }

    SetPackageSearchPaths("/home/henry/apoapsis;batman");
    MountPackage("core");
    Log("------------");
    UnmountPackage("core");

    const char* error = PHYSFS_getLastError();
    if(!error)
    {
        return true;
    }
    else
    {
        Error("%s", error);
        return false;
    }
}

void DestroyPhysFS()
{
    if(!PHYSFS_deinit())
        Error("%s", PHYSFS_getLastError());
}

const char* GetUserDataDirectory()
{
    return UserDataDirectory;
}

void SetPackageSearchPaths( const char* paths )
{
    CopyString(paths, SearchPaths, sizeof(SearchPaths));
}

static const char* ResolvePackageNameWithBasePath( const char* name,
                                                   const char* basePath )
{
    static char buffer[MAX_PATH_LENGTH+1];

    if(strlen(basePath) == 0)
        return NULL;

    const char* separator = PHYSFS_getDirSeparator();
    const char* path = NULL;

    path = Format("%s%s%s", basePath, separator, name);
    if(GetFileType(path) != FILE_TYPE_DIRECTORY)
        path = NULL;

    path = Format("%s%s%s.zip", basePath, separator, name);
    if(GetFileType(path) != FILE_TYPE_REGULAR)
        path = NULL;

    if(path)
    {
        CopyString(path, buffer, sizeof(buffer));
        return buffer;
    }
    else
    {
        return NULL;
    }
}

static const char* ResolvePackageNameWithSearchPath( const char* name,
                                                     const char* searchPath )
{
    const int searchPathLength = strlen(searchPath);
    int pathStart = 0;
    for(int i = 0; i < searchPathLength; i++)
    {
        if(searchPath[i] == PATH_SEPARATOR)
        {
            const int pathEnd = (i-1);
            const int pathLength = (i-pathStart);
            assert(pathLength >= 0);
            assert(pathLength < MAX_PATH_LENGTH);

            char buffer[MAX_PATH_LENGTH+1];
            memcpy(buffer, &searchPath[pathStart], pathLength);
            buffer[pathLength] = '\0';

            const char* path = ResolvePackageNameWithBasePath(name, buffer);
            if(path)
                return path;

            pathStart = i+1;
        }
    }
    return ResolvePackageNameWithBasePath(name, &searchPath[pathStart]);
}

bool MountPackage( const char* name )
{
    const char* packagePath = ResolvePackageNameWithSearchPath(name, SearchPaths);

    if(!packagePath)
        packagePath =
            ResolvePackageNameWithSearchPath(name, DEFAULT_PACKAGE_SEARCH_PATHS);

    if(packagePath)
    {
        const char* mountPath = Format("/%s", name);
        if(PHYSFS_mount(packagePath, mountPath, true))
        {
            Log("Mounted package '%s' (%s).", name, packagePath);
            return true;
        }
        else
        {
            Error("Can't mount package '%s' (%s):", name, packagePath);
            Error("%s", PHYSFS_getLastError());
            return false;
        }
    }
    else
    {
        Error("Can't find package '%s' in search paths.", name);
        return false;
    }
}

void UnmountPackage( const char* name )
{
    const char* mountPath = Format("/%s", name);
    const char* packagePath = PHYSFS_getRealDir(mountPath);
    if(packagePath)
    {
        if(PHYSFS_unmount(packagePath))
        {
            Log("Unmounted package '%s'.", name);
        }
        else
        {
            Error("Can't unmount package '%s' (%s):", name, packagePath);
            Error("%s", PHYSFS_getLastError());
        }
    }
    else
    {
        Error("Can't unmount package '%s' since it's not loaded.", name);
    }
}
