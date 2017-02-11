#include <string.h> // strcmp
#include "../Common.h"
#include "TestTools.h"


void FormatTest()
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

void LogHandlerTest()
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
                Require(level == LOG_WARNING);
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
    Warn("bar");
    Log("aaa\nbbb");
    Log("ccc\n");

    Require(CallCount == 6);
}


void CopyStringTest()
{
    static const int maxLength = 7;
    char destination[maxLength+1];

    static const char* source = "0123456";
    Require(CopyString(source, destination, sizeof(destination)) == true);
    Require(memcmp(source, destination, sizeof(destination)) == 0);

    static const char* smallSource = "0123";
    static const char* smallSourceDestination = "0123\0\0\0";
    Require(CopyString(smallSource, destination, sizeof(destination)) == true);
    Require(memcmp(smallSourceDestination, destination, sizeof(destination)) == 0);

    static const char* largeSource = "012345678";
    static const char* largeSourceDestination = "0123456";
    Require(CopyString(largeSource, destination, sizeof(destination)) == false);
    Require(memcmp(largeSourceDestination, destination, sizeof(destination)) == 0);
}


int main( int argc, char** argv )
{
    InitTests(argc, argv);
    AddTest("Format() works correct.", FormatTest);
    AddTest("Log handler can handle log lines.", LogHandlerTest);
    AddTest("CopyString() is correct.", CopyStringTest);
    return RunTests();
}
