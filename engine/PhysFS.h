#ifndef __APOAPSIS_PHYSFS__
#define __APOAPSIS_PHYSFS__

bool InitPhysFS( const int argc, char const * const * argv );
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
 * Resolves the package name to an archive or directory and mounts it.
 *
 * Once mounted, the packages contents will be available under
 * `/<package name>` in the virtual file system.
 * Among other things, the package search path will be used to resolve a
 * package name.  See #SetPackageSearchPaths for more information.
 *
 * @return
 * `true` if the package name could be resolved and the package was
 * successfully mounted.  `false` otherwise.
 */
bool MountPackage( const char* name );

/**
 * Releases a mounted package and makes it unavailable.
 *
 * The package won't be accessible through the virtual file system anymore.
 * Nothing will happen, if no package with this name has been mounted.
 */
void UnmountPackage( const char* name );

#endif
