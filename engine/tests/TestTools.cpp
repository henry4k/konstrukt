#include <assert.h>
#include <stdlib.h> // atexit
#include <string.h> // strrchr, strncpy
#include <stdio.h> // stdout
#include <dummy/tap_reporter.h>

#include "../Common.h"
#include "../Vfs.h"

#include "TestTools.h"


void ChangeDirectoryToExecutableOrigin( const char* executablePath );

void TestLogHandler( LogLevel level, const char* line )
{
    switch(level)
    {
        case LOG_INFO:
        case LOG_NOTICE:
            dummyLog("%s", line);
            break;

        case LOG_WARNING:
            dummyLog("WARNING: %s", line);
            break;

        case LOG_ERROR:
            dummyLog("ERROR: %s", line);
            break;

        case LOG_FATAL_ERROR:
            dummyLog("FATAL ERROR: %s", line);
            break;

        default:
            assert(!"Unknown log level");
    }
}

void InitTests( int argc, char const * const * argv )
{
    ChangeDirectoryToExecutableOrigin(argv[0]);
    SetLogHandler(TestLogHandler);
    dummyInit(dummyGetTAPReporter(stdout));
}

int RunTests()
{
    dummyRunTests();
    return 0;
}

#if defined(_WIN32)

#include <direct.h>
void ChangeDirectory( const char* directory )
{
    _chdir(directory);
}

#else

#include <unistd.h>
void ChangeDirectory( const char* directory )
{
    chdir(directory);
}

#endif

void ChangeDirectoryToExecutableOrigin( const char* executablePath )
{
#if defined(_WIN32)
    const char seperator = '\\';
#else
    const char seperator = '/';
#endif
    const char* lastSeperator = strrchr(executablePath, seperator);
    if(lastSeperator)
    {
        char buffer[256];
        const int length = lastSeperator - executablePath;
        strncpy(buffer, executablePath, length);
        buffer[length] = '\0';
        ChangeDirectory(buffer);
    }
    else
    {
        // Already in the directory of the executable.
    }
}

void InitTestVfs( const char* argv0 )
{
    InitVfs(argv0, NULL, NULL);
    atexit(DestroyVfs);
}
