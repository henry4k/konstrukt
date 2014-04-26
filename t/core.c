#include <stdlib.h> // malloc, free
#include <stddef.h> // NULL
#include <assert.h>

#include "../dummy/core.h"
#include "../dummy/signal_runner.h"
#include "../dummy/require.h"

#include "test_reporter.h"


void PassingTest()
{
}

void AssertTest()
{
    assert(0);
}

void FailTest()
{
    dummyAbortTest(DUMMY_FAIL_TEST, NULL);
}

void FailWithReasonTest()
{
    dummyAbortTest(DUMMY_FAIL_TEST, "Failed with a reason");
}

void PassingRequireTest()
{
    dummyRequire(2+2 == 4);
}

void FailingRequireTest()
{
    dummyRequire(2+2 == 5);
}

void SkipTest()
{
    dummyAbortTest(DUMMY_SKIP_TEST, NULL);
}

void SkipWithReasonTest()
{
    dummyAbortTest(DUMMY_SKIP_TEST, "Skipped with a reason");
}

void TodoTest()
{
    dummyMarkTestAsTodo(NULL);
}

void TodoWithReasonTest()
{
    dummyMarkTestAsTodo("TODO with a reason");
}

int* CreateInt( int value )
{
    int* value_ = malloc(sizeof(int));
    *value_ = value;
    dummyLog("created %d", value);
    return value_;
}

void CleanupInt( void* value_ )
{
    int* value = (int*)value_;
    dummyLog("cleaned %d", *value);
    free(value);
}

void PassingCleanupTest()
{
    void* data = CreateInt(42);
    dummyAddCleanup(CleanupInt, data);
}

void FailingCleanupTest()
{
    void* data = CreateInt(42);
    dummyAddCleanup(CleanupInt, data);
    dummyAbortTest(DUMMY_FAIL_TEST, NULL);
}

int main()
{
    dummyInit(dummyGetSignalRunner(), dummyGetTestReporter());
    dummyAddTest("PassingTest", PassingTest);
    dummyAddTest("AssertTest", AssertTest);
    dummyAddTest("FailTest", FailTest);
    dummyAddTest("FailWithReasonTest", FailWithReasonTest);
    dummyAddTest("PassingRequireTest", PassingRequireTest);
    dummyAddTest("FailingRequireTest", FailingRequireTest);
    dummyAddTest("SkipTest", SkipTest);
    dummyAddTest("SkipWithReasonTest", SkipWithReasonTest);
    dummyAddTest("TodoTest", TodoTest);
    dummyAddTest("TodoWithReasonTest", TodoWithReasonTest);
    dummyAddTest("PassingCleanupTest", PassingCleanupTest);
    dummyAddTest("FailingCleanupTest", FailingCleanupTest);
    dummyRunTests();
    return 0;
}
