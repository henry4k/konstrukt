#include <string.h> // strlen, memset, memcpy
#include "Common.h" // CopyString
#include "Game.h" // DEFAULT_PACKAGE_SEARCH_PATHS
#include "Config.h"
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

bool PostConfigInitPhysFS()
{
    PHYSFS_permitSymbolicLinks(GetConfigBool("package.permit-symbolic-links", false));
    SetPackageSearchPaths(GetConfigString("package.search-paths", ""));
    return true;
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
    const char* finalPaths = Format("%s;%s", paths, DEFAULT_PACKAGE_SEARCH_PATHS);
    CopyString(finalPaths, SearchPaths, sizeof(SearchPaths));
    Log("Package search paths: %s", SearchPaths);
}

static const char* ResolvePackageNameWithBasePath( const char* name,
                                                   const char* basePath )
{
    static char buffer[MAX_PATH_LENGTH+1];

    if(strlen(basePath) == 0)
        return NULL;

    const char* separator = PHYSFS_getDirSeparator();
    const char* path = NULL;

    if(!path)
    {
        path = Format("%s%s%s", basePath, separator, name);
        if(GetFileType(path) != FILE_TYPE_DIRECTORY)
            path = NULL;
    }

    if(!path)
    {
        path = Format("%s%s%s.zip", basePath, separator, name);
        if(GetFileType(path) != FILE_TYPE_REGULAR)
            path = NULL;
    }

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

static const char* ResolvePackageName( const char* name )
{
    return ResolvePackageNameWithSearchPath(name, SearchPaths);
}

bool MountPackage( const char* name )
{
    const char* packagePath = ResolvePackageName(name);
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
    const char* packagePath = ResolvePackageName(name);
    // TODO: This is an error prone solution.  It can break when you change the search path.
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

FileBuffer* LoadFile( const char* vfsPath )
{
    PHYSFS_File* file = PHYSFS_openRead(vfsPath);
    if(!file)
    {
        Error("Can't open file '%s': %s", vfsPath, PHYSFS_getLastError());
        return 0;
    }

    const int size = (int)PHYSFS_fileLength(file);

    char* buffer = new char[size];
    const int bytesRead = PHYSFS_readBytes(file, buffer, size);
    PHYSFS_close(file);

    if(bytesRead == size)
    {
        return new FileBuffer { size, buffer };
    }
    else
    {
        Error("Can't read file '%s': %s", vfsPath, PHYSFS_getLastError());
        delete[] buffer;
        return NULL;
    }
}

void FreeFile( FileBuffer* buffer )
{
    delete[] buffer->data;
    delete buffer;
}
