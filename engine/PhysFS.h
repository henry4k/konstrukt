#ifndef __KONSTRUKT_PHYSFS__
#define __KONSTRUKT_PHYSFS__

#include <physfs.h>

// PhysFS backwards compatibility:
#if PHYSFS_VER_MAJOR < 2 || (PHYSFS_VER_MAJOR == 2 && PHYSFS_VER_MINOR == 0)
#define PHYSFS_NO_STAT
#define PHYSFS_readBytes( handle, buffer, size ) PHYSFS_read((handle), (buffer), 1, (size))
#define PHYSFS_writeBytes( handle, buffer, size ) PHYSFS_write((handle), (buffer), 1, (size))
#define PHYSFS_unmount PHYSFS_removeFromSearchPath
#endif


struct FileBuffer
{
    int size;
    char* data;
};


bool InitPhysFS( const char* argv0 );
bool PostConfigInitPhysFS();
void DestroyPhysFS();

/**
 * Directory where user specific files may be stored.
 *
 * E.g. configuration, save games, downloaded packages and so on.
 */
const char* GetUserDataDirectory();

/**
 * A path list which is used to find packages.
 *
 * The paths are separated by semicolons (;).
 * To resolve a package name each search path is tried with these combinations:
 * 1. `<search path>/<package name>`
 * 2. `<search path>/<package name>.zip`
 * These search paths are tried first, but if none of them applies, other
 * things may be tried to resolve a package name.
 */
void SetPackageSearchPaths( const char* paths );

/**
 * Resolves the package reference to an archive or directory and mounts it.
 *
 * Once mounted, the packages contents will be available under
 * `/<package name>` in the virtual file system.
 * Among other things, the package search path will be used to resolve a
 * package name.  See #SetPackageSearchPaths for more information.
 *
 * @param reference
 * Is either a file path to a package or a package id, which can be found in
 * one of the search paths.
 *
 * @return
 * `true` if the package was found and successfully mounted.  `false` otherwise.
 */
bool MountPackage( const char* reference );

/**
 * Helper function that loads a complete vfs file into memory.
 *
 * @return
 * A #FileBuffer object or `NULL` if something went wrong.
 */
FileBuffer* LoadFile( const char* vfsPath );

/**
 * Use to free a file buffer.
 */
void FreeFile( FileBuffer* buffer );

#endif
