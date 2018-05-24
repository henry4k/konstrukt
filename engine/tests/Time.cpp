#include <string.h> // strcmp
#include "../Time.h"
#include "TestTools.h"


class TimeScope
{
private:
    bool destroyed;

public:
    TimeScope() { destroyed = false; }
    ~TimeScope() { if(!destroyed) DestroyTime(); }
    void destroy() { DestroyTime(); destroyed = true; };
};

static const void* ExpectedTimerContext = NULL;
static int   TimerCallCount = 0;
static double LastTimeDelta = 0;
static void MyTimerCallback( Timer* timer, double timeDelta, void* context )
{
    Require(context == ExpectedTimerContext);
    TimerCallCount++;
    LastTimeDelta = timeDelta;
}


InlineTest("can be initialized and destroyed.")
{
    InitTime();
    DestroyTime();
}

InlineTest("can advance simulation time.")
{
    InitTime();
    TimeScope scope;

    Require(GetTime() == 0);
    UpdateTime(1);
    Require(GetTime() == 1);
    UpdateTime(1);
    Require(GetTime() == 2);
}

InlineTest("can create and destroy timers.")
{
    InitTime();
    TimeScope scope;
    UpdateTime(100);

    int context = 42;
    ExpectedTimerContext = &context;
    Timer* myTimer = CreateTimer(10, &context, MyTimerCallback);
    Require(TimerCallCount == 0);

    UpdateTime(1);
    Require(TimerCallCount == 0);

    UpdateTime(9);
    Require(TimerCallCount == 1);
    Require(LastTimeDelta == 10);

    UpdateTime(10);
    Require(TimerCallCount == 2);
    Require(LastTimeDelta == 10);

    UpdateTime(11);
    Require(TimerCallCount == 3);
    Require(LastTimeDelta == 11);

    DestroyTimer(myTimer);
    UpdateTime(100);
    Require(TimerCallCount == 3);
}

int main( int argc, char** argv )
{
    InitTests(argc, argv);
    return RunTests();
}
