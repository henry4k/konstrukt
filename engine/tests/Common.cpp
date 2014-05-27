#include <string.h> // strcmp
#include <engine/Common.h>
#include "TestTools.h"


InlineTest("Format() works correct.", dummySignalSandbox)
{
    Require(strcmp(Format("foo"), "foo") == 0);
    Require(strcmp(Format("bar%d",1), "bar1") == 0);

    const char* aaa = Format("aaa");
    Require(strcmp(aaa, "aaa") == 0);
    Format("bbb");
    Require(strcmp(aaa, "aaa") != 0);
}


LogHandler OriginalLogHandler;
int CallCount;

InlineTest("Log handler can handle log lines.", dummySignalSandbox)
{
    OriginalLogHandler = GetLogHandler();
    dummyAddCleanup(
        []( void* _ ){ SetLogHandler(OriginalLogHandler); },
        NULL);

    SetLogHandler([]( LogLevel level, const char* line ){
        dummyLog("level=%d line='%s'", level, line);
        CallCount++;
        switch(CallCount)
        {
            case 1:
                Require(level == LOG_INFO);
                Require(strcmp(line, "foo") == 0);
                break;

            case 2:
                Require(level == LOG_ERROR);
                Require(strcmp(line, "bar") == 0);
                break;

            case 3:
                Require(level == LOG_INFO);
                Require(strcmp(line, "aaa") == 0);
                break;

            case 4:
                Require(level == LOG_INFO);
                Require(strcmp(line, "bbb") == 0);
                break;

            case 5:
                Require(level == LOG_INFO);
                Require(strcmp(line, "ccc") == 0);
                break;

            case 6:
                Require(level == LOG_INFO);
                Require(strcmp(line, "") == 0);
                break;

            default:
                dummyAbortTest(DUMMY_FAIL_TEST,
                    "Invalid log handler call count: %d", CallCount);
        }
    });

    CallCount = 0;

    Log("foo");
    Error("bar");
    Log("aaa\nbbb");
    Log("ccc\n");

    Require(CallCount == 6);
}


int main( int argc, char** argv )
{
    InitTests(argc, argv);
    return RunTests();
}
