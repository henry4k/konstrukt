#include <dummy/exception_runner.h>
#include <dummy/tap_reporter.h>

#include <src/Common.h>

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
    dummyInit(dummyGetExceptionRunner(), dummyGetTAPReporter());
}

int RunTests()
{
    const int failedTests = dummyRunTests();
    SetLogHandler(DefaultLogHandler);
    return failedTests;
}
