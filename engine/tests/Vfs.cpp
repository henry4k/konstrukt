#include <string.h> // strcmp
#include "../Common.h"
#include "../Vfs.h"
#include "TestTools.h"


InlineTest("VFS can be initialized and destroyed.")
{
    InitVfs("test", NULL, NULL);
    DestroyVfs();
}

static void TestReadTextFile( const char* path )
{
    VfsFile* file = OpenVfsFile(path, VFS_OPEN_READ);

    // "hello world\n" = 12 bytes
    Require(GetVfsFileSize(file) == 12);

    Require(!HasVfsFileEnded(file));

    char buffer[13];
    buffer[12] = '\0';
    ReadVfsFile(file, buffer, 12);
    Require(strcmp(buffer, "hello world\n") == 0);

    ReadVfsFile(file, buffer, 1);
    Require(HasVfsFileEnded(file));

    CloseVfsFile(file);
}
static bool IsEntryInPathList( const PathList list, const char* entry )
{
    REPEAT(list.length, i)
        if(strcmp(list.data[i].str, entry) == 0)
            return true;
    return false;
}

static void TestDirEntries( const char* path,
                            const char** entries,
                            int entryCount )
{
    LogInfo("%s:", path);
    PathList list = GetVfsDirEntries(path);
    REPEAT(list.length, i)
        LogInfo("\t%s", list.data[i].str);

    Require(list.length == entryCount);

    REPEAT(entryCount, i)
        Require(IsEntryInPathList(list, entries[i]));

    DestroyPathList(&list);
}

static void TestReadingMount()
{
    Require(GetVfsFileType("") == FILE_TYPE_DIRECTORY);
    Require(GetVfsFileType("package") == FILE_TYPE_DIRECTORY);
    Require(GetVfsFileType("package/textfile") == FILE_TYPE_REGULAR);
    Require(GetVfsFileType("package/subdir") == FILE_TYPE_DIRECTORY);
    Require(GetVfsFileType("package/subdir/textfile") == FILE_TYPE_REGULAR);

    TestReadTextFile("package/textfile");
    TestReadTextFile("package/subdir/textfile");

    {
        const char* entries[] = {"package", "state", "shared-state"};
        TestDirEntries("", entries, 3);
    }

    {
        const char* entries[] = {"textfile", "subdir"};
        TestDirEntries("package", entries, 2);
    }

    {
        const char* entries[] = {"textfile"};
        TestDirEntries("package/subdir", entries, 1);
    }
}

InlineTest("Can mount real directories for reading.")
{
    InitVfs("test", NULL, NULL);
    MountVfsDir("package", "data/package-dir.1.2.3", false);
    TestReadingMount();
    DestroyVfs();
}

InlineTest("Can mount ZIP archives for reading.")
{
    InitVfs("test", NULL, NULL);
    MountVfsDir("package", "data/package-zip.1.2.3.zip", false);
    TestReadingMount();
    DestroyVfs();
}

InlineTest("Can mount real directories for writing.")
{
    InitVfs("test", NULL, NULL);
    MountVfsDir("package", "data/package-dir.1.2.3", true);
    TestReadingMount();
    DestroyVfs();
}

int main( int argc, char** argv )
{
    InitTests(argc, argv);
    return RunTests();
}
