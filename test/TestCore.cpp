#include "TestCore.h"

void TestLogHandler( LogLevel level, const char* line )
{
    switch(level)
    {
        case LOG_INFO:
            note("%s", line);
            break;

        case LOG_ERROR:
        case LOG_FATAL_ERROR:
            diag("%s", line);
            break;

        default:
            assert(!"Unknown log level");
    }
}

void SetupTestCore()
{
    SetLogHandler(TestLogHandler);
}

void TeardownTestCore()
{
}
