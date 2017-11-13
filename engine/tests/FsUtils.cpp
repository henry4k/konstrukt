#include <string.h> // strcmp
#include "../Common.h" // REPEAT
#include "../FsUtils.h"
#include "TestTools.h"
#include <dummy/inline.hpp>

#define InlineTest DUMMY_INLINE_TEST

InlineTest("GetFileType", dummySignalSandbox)
{
    Require(GetFileType("data/FsUtils")     == FILE_TYPE_DIRECTORY);
    Require(GetFileType("data/FsUtils/aaa") == FILE_TYPE_REGULAR);
    Require(GetFileType("data/FsUtils/zzz") == FILE_TYPE_INVALID);
}

static bool HasPath( const PathList list, const char* path )
{
    REPEAT(list.length, i)
        if(strcmp(list.data[i].str, path) == 0)
            return true;
    return false;
}

InlineTest("GetDirEntries", dummySignalSandbox)
{
    PathList list = GetDirEntries("data/FsUtils");
    Require(list.length == 2);
    Require(HasPath(list, "aaa"));
    Require(HasPath(list, "bbb"));
    DestroyPathList(&list);
}

int main( int argc, char** argv )
{
    InitTests(argc, argv);
    dummyAddInlineTests();
    return RunTests();
}
