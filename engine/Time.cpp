#include <assert.h>
#include <string.h> // memset

#include "Common.h"
#include "Profiler.h"
#include "Time.h"


static const int MAX_TIMERS = 64;


struct Timer
{
    double minDelay;
    double lastOccurrence;
    TimerCallback callback;
    void* context;
};


static double Time;
static Timer Timers[MAX_TIMERS];


static bool TimerIsActive( const Timer* timer );


void InitTime()
{
    assert(InSerialPhase());
    Time = 0;
    memset(Timers, 0, sizeof(Timers));
}

void DestroyTime()
{
    assert(InSerialPhase());
}

void UpdateTime( double timeDelta )
{
    ProfileFunction();

    assert(timeDelta >= 0);
    Time += timeDelta;
    REPEAT(MAX_TIMERS, i)
    {
        Timer* timer = &Timers[i];
        if(TimerIsActive(timer))
        {
            const double timerTimeDelta = Time - timer->lastOccurrence;
            if(timerTimeDelta >= timer->minDelay)
            {
                timer->lastOccurrence = Time;
                timer->callback(timer, timerTimeDelta, timer->context);
            }
        }
    }
}

double GetTime()
{
    return Time;
}

static bool TimerIsActive( const Timer* timer )
{
    return timer->callback != NULL;
}

static Timer* FindInactiveTimer()
{
    for(int i = 0; i < MAX_TIMERS; i++)
    {
        Timer* timer = &Timers[i];
        if(!TimerIsActive(timer))
            return timer;
    }
    return NULL;
}

Timer* CreateTimer( double minDelay, void* context, TimerCallback callback )
{
    assert(InSerialPhase());
    assert(minDelay >= 0);
    assert(callback);

    Timer* timer = FindInactiveTimer();
    if(!timer)
        FatalError("Can't create more timers.");

    memset(timer, 0, sizeof(Timer));
    timer->minDelay = minDelay;
    timer->lastOccurrence = Time;
    timer->callback = callback;
    timer->context = context;
    return timer;
}

void DestroyTimer( Timer* timer )
{
    assert(InSerialPhase());
    assert(timer);
    memset(timer, 0, sizeof(Timer));
}
