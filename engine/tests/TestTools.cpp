#include <assert.h>
#include <stdlib.h> // atexit
#include <string.h> // strrchr, strncpy
#include <stdio.h> // stdout
#include <dummy/tap_reporter.h>

#include "../Common.h"
#include "../Vfs.h"
#include "../Config.h"
#include "../JobManager.h"

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

static void InitTestConfig( int argc, char const * const * argv )
{
    InitConfig();
    atexit(DestroyConfig);

    for(int i = 1; i < argc; i++)
    {
        const char* arg = argv[i];
        const char* match;

        match = MatchPrefix("--config=", arg);
        if(match) { ReadConfigFile(match, true); continue; }

        match = MatchPrefix("-D", arg);
        if(match) { ReadConfigString(match); continue; }
    }
}

void InitTests( int argc, char const * const * argv )
{
    InitTestConfig(argc, argv);
    SetLogHandler(TestLogHandler);
    dummyInit(dummyGetTAPReporter(stdout));
    dummyAddInlineTests();
}

int RunTests()
{
    dummyRunTests();
    return 0;
}

void InitTestVfs( const char* argv0 )
{
    InitVfs(argv0, NULL, NULL);
    atexit(DestroyVfs);
}

void InitTestJobManager()
{
    JobManagerConfig managerConfig;
    managerConfig.workerThreads = 3;
    InitJobManager(managerConfig);
    atexit(DestroyJobManager);
}
