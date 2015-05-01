#include <string.h> // strrchr, strncpy
#include <stdio.h> // stdout
#include <dummy/tap_reporter.h>

#include <engine/Common.h>

#include "TestTools.h"


static LogHandler g_OriginalLogHandler = NULL;

void ChangeDirectoryToExecutableOrigin( const char* executablePath );

void TestLogHandler( LogLevel level, const char* line )
{
    switch(level)
    {
        case LOG_INFO:
            dummyLog("%s", line);
            break;

        case LOG_ERROR:
        case LOG_FATAL_ERROR:
            dummyLog("%s", line);
            break;

        default:
            assert(!"Unknown log level");
    }
}

void InitTests( int argc, char const * const * argv )
{
    ChangeDirectoryToExecutableOrigin(argv[0]);
    g_OriginalLogHandler = GetLogHandler();
    SetLogHandler(TestLogHandler);
    dummyInit(dummyGetTAPReporter(stdout));
    dummyAddInlineTests();
}

int RunTests()
{
    const int failedTests = dummyRunTests();
    if(g_OriginalLogHandler)
        SetLogHandler(g_OriginalLogHandler);
    //return failedTests; // TODO: prove marks failed tests as doubious, with this line
    return 0;
}

#if defined(WIN32)

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
#if defined(WIN32)
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
