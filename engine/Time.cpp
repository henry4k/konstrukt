#include <assert.h>
#include <string.h> // memset
#include "Common.h"
#include "Time.h"


static const int MAX_TIMERS = 8;


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


bool InitTime()
{
    Time = 0;
    memset(Timers, 0, sizeof(Timers));
    return true;
}

void DestroyTime()
{
}

void UpdateTime( double timeDelta )
{
    assert(timeDelta >= 0);
    Time += timeDelta;
    for(int i = 0; i < MAX_TIMERS; i++)
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
    assert(minDelay >= 0);
    assert(callback);

    Timer* timer = FindInactiveTimer();
    if(timer)
    {
        memset(timer, 0, sizeof(Timer));
        timer->minDelay = minDelay;
        timer->lastOccurrence = Time;
        timer->callback = callback;
        timer->context = context;
        return timer;
    }
    else
    {
        Error("Can't create more timers.");
        return NULL;
    }
}

void DestroyTimer( Timer* timer )
{
    assert(timer);
    memset(timer, 0, sizeof(Timer));
}
