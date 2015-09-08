#include <string.h> // strcmp
#include <engine/Time.h>
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
static void MyTimerCallback( Timer* timer, void* context )
{
    Require(context == ExpectedTimerContext);
    TimerCallCount++;
}

int main( int argc, char** argv )
{
    InitTests(argc, argv);

    Describe("Test module")
        .use(dummyExceptionSandbox)

        .it("can be initialized and destroyed.", [](){
            Require(InitTime() == true);
            DestroyTime();
        })

        .it("can advance simulation time.", [](){
            InitTime();
            TimeScope scope;

            Require(GetTime() == 0);
            UpdateTime(1);
            Require(GetTime() == 1);
            UpdateTime(1);
            Require(GetTime() == 2);
        })

        .it("can create and destroy timers.", [](){
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

            UpdateTime(10);
            Require(TimerCallCount == 2);

            //UpdateTime(100);
            //Require(TimerCallCount == 12);

            DestroyTimer(myTimer);
            UpdateTime(100);
            Require(TimerCallCount == 2);
        });

    return RunTests();
}
