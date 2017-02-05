#include <assert.h>
#include <string.h> // strlen, memset, memcpy

#include "Common.h" // CopyString
#include "Constants.h" // DEFAULT_PACKAGE_SEARCH_PATHS
#include "Config.h"
#include "PhysFS.h"




enum FileType
{
    FILE_TYPE_INVALID,
    FILE_TYPE_UNKNOWN,
    FILE_TYPE_REGULAR,
    FILE_TYPE_DIRECTORY
};

static FileType GetFileType( const char* path )
{
    return FILE_TYPE_INVALID;
}


static const int MAX_PATH_SIZE = 256;
static const int MAX_PATH_LIST_SIZE = 512;
static const char PATH_SEPARATOR = ';';


static char UserDataDirectory[MAX_PATH_SIZE];
static char SearchPaths[MAX_PATH_LIST_SIZE];


static void LogPhysFSVersion()
{
    PHYSFS_Version compiled;
    PHYSFS_Version linked;

    PHYSFS_VERSION(&compiled);
    PHYSFS_getLinkedVersion(&linked);

    Log("Using PhysFS %d.%d.%d", compiled.major, compiled.minor, compiled.patch);
    Log("Linked against PhysFS %d.%d.%d", linked.major, linked.minor, linked.patch);
}

#if defined(KONSTRUKT_SELFCONTAINED)
static bool SetupConfig()
{
    return PHYSFS_setWriteDir(PHYSFS_getBaseDir());
}
#else
static bool SetupConfig()
{
    if(PHYSFS_setSaneConfig("konstrukt", // Organization
                            "konstrukt", // Program name
                            NULL, // Extension of automatically mounted archives
                            0, // Include CD-ROMs
                            0)) // Prepend automatically found archives
    {
        // Base path is mounted by PHYSFS_setSaneConfig, but we don't use it.
        PHYSFS_unmount(PHYSFS_getBaseDir());
        return true;
    }
    else
    {
        return false;
    }
}
#endif

bool InitPhysFS( const char* argv0 )
{
    memset(&UserDataDirectory, 0, sizeof(UserDataDirectory));
    memset(SearchPaths,        0, sizeof(SearchPaths));
    CopyString(DEFAULT_PACKAGE_SEARCH_PATHS, SearchPaths, sizeof(SearchPaths));

    LogPhysFSVersion();
    if(PHYSFS_init(argv0) &&
       SetupConfig())
    {
        assert(PHYSFS_getWriteDir() != NULL);
        CopyString(PHYSFS_getWriteDir(), UserDataDirectory, sizeof(UserDataDirectory));
        const int pathLength = strlen(UserDataDirectory);
        if(UserDataDirectory[pathLength-1] == '/' || UserDataDirectory[pathLength-1] == '\\')
            UserDataDirectory[pathLength-1] = '\0'; // Clip the trailing directory separator.

        Log("User data directory is '%s'", UserDataDirectory);
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

static const char* ResolvePackageIdWithBasePath( const char* id,
                                                 const char* basePath )
{
    static char buffer[MAX_PATH_SIZE];

    if(strlen(basePath) == 0)
        return NULL;

    const char* separator = PHYSFS_getDirSeparator();
    const char* path = NULL;

    if(!path)
    {
        path = Format("%s%s%s", basePath, separator, id);
        if(GetFileType(path) != FILE_TYPE_DIRECTORY)
            path = NULL;
    }

    if(!path)
    {
        path = Format("%s%s%s.zip", basePath, separator, id);
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

static const char* ResolvePackageIdWithSearchPath( const char* id,
                                                   const char* searchPath )
{
    const int searchPathLength = strlen(searchPath);
    int pathStart = 0;
    for(int i = 0; i < searchPathLength; i++)
    {
        if(searchPath[i] == PATH_SEPARATOR)
        {
            const int pathLength = (i-pathStart);
            assert(pathLength >= 0);
            assert(pathLength < MAX_PATH_SIZE-1);

            char buffer[MAX_PATH_SIZE];
            memcpy(buffer, &searchPath[pathStart], pathLength);
            buffer[pathLength] = '\0';

            const char* path = ResolvePackageIdWithBasePath(id, buffer);
            if(path)
                return path;

            pathStart = i+1;
        }
    }
    return ResolvePackageIdWithBasePath(id, &searchPath[pathStart]);
}

static const char* ResolvePackageReference( const char* reference )
{
    if(GetFileType(reference) != FILE_TYPE_DIRECTORY)
        return reference;
    if(GetFileType(reference) != FILE_TYPE_REGULAR)
        return reference;
    return ResolvePackageIdWithSearchPath(reference, SearchPaths);
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
        const char* mountPath = ExtractPackageNameFromReference(reference);
        if(PHYSFS_mount(packagePath, mountPath, true))
        {
            Log("Mounted package '%s' (%s).", reference, packagePath);
            return true;
        }
        else
        {
            Error("Can't mount package '%s' (%s):", reference, packagePath);
            Error("%s", PHYSFS_getLastError());
            return false;
        }
    }
    else
    {
        Error("Can't find package '%s' in search paths.", reference);
        return false;
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
        FileBuffer* file = new FileBuffer;
        file->size = size;
        file->data = buffer;
        return file;
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
