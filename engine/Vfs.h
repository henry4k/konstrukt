#ifndef __KONSTRUKT_VFS__
#define __KONSTRUKT_VFS__

#include "FsUtils.h" // PathList, FileType, MAX_PATH_SIZE


enum VfsOpenMode
{
    VFS_OPEN_READ,
    VFS_OPEN_WRITE,
    VFS_OPEN_APPEND,
};


/**
 * A file accessed through the virtual file system.
 * It either exists in an archive or in the real file system.
 */
struct VfsFile;


/**
 * @param stateDirectory
 * Path to a directory in which the simulation state can be stored.  Passing
 * `NULL` will create temporary directory.
 *
 * @param sharedStateDirectory
 * Path to a directory which stores data, that is shared by all/multiple
 * scenarios.  Passing `NULL` will create temporary directory.
 */
void InitVfs( const char* argv0,
              const char* stateDirectory,
              const char* sharedStateDirectory );

void DestroyVfs();


// --- Mounting ---

/**
 * Mount a ZIP archive or an actual directory in the virtual file system.
 *
 * @param vfsPath
 * Path to the mount point.  At the moment mount points **must** be located at
 * the file system root.
 *
 * @param realPath
 * Native path to the archive or directory.
 *
 * @param writingAllowed
 * If set to `false` the VFS may not write to this directory.
 * This can't be used with ZIP archives.
 */
void MountVfsDir( const char* vfsPath,
                  const char* realPath,
                  bool writingAllowed );

/**
 * Remove a mount point from the virtual file system.
 *
 * @param vfsPath
 * Path to the mount point.
 */
void UnmountVfsDir( const char* vfsPath );


// --- Package mount utilities ---

/**
 * Appends a directory path to the search path list.
 *
 * When mounting the search path list is traversed form the start to the end.
 */
void AddPackageSearchPath( const char* path );

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
 */
void MountPackage( const char* reference );


// --- File access ---

/**
 * @param path
 * VFS path to the file.
 *
 * @param mode
 * The same mode flags that are accepted by `fopen`.
 */
VfsFile* OpenVfsFile( const char* vfsPath, VfsOpenMode mode );

void CloseVfsFile( VfsFile* file );

/**
 * Read bytes from a file.
 *
 * @return
 * Number of bytes read.
 */
int ReadVfsFile( VfsFile* file, void* buffer, int size );

/**
 * Write bytes to a file.
 *
 * @return
 * Number of bytes written.
 */
int WriteVfsFile( VfsFile* file, const void* buffer, int size );

void SetVfsFilePos( VfsFile* file, int position );
int  GetVfsFilePos( const VfsFile* file );
int  GetVfsFileSize( const VfsFile* file );
bool HasVfsFileEnded( const VfsFile* file );


// --- File system introspection ---

/**
 * Lists all files in a directory.  Only real entries are emitted, nothing
 * like `..` or `.`.
 *
 * @return
 * A list of directory entries.
 * Use #DestroyPathList when you don't need it anymore.
 */
PathList GetVfsDirEntries( const char* vfsPath );

/**
 * Retrieve information about a file.
 *
 * If the file doesn't exists `FILE_TYPE_INVALID` is returned.
 */
FileType GetVfsFileType( const char* vfsPath );

/**
 * Tries to delete a regular file or an empty directory.
 */
void DeleteVfsFile( const char* vfsPath );

void MakeVfsDir( const char* vfsPath );

#endif
