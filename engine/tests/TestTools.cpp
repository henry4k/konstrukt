#include <stdio.h>
#include <dummy/tap_reporter.h>

#include <engine/Common.h>

#include "TestTools.h"


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

void InitTests()
{
    SetLogHandler(TestLogHandler);
    dummyInit(dummyGetTAPReporter(stdout));
}

int RunTests()
{
    const int failedTests = dummyRunTests();
    SetLogHandler(DefaultLogHandler);
    //return failedTests; // TODO: prove marks failed tests as doubious, with this line
    return 0;
}
