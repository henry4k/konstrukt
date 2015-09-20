#ifndef __APOAPSIS_TIME__
#define __APOAPSIS_TIME__

/**
 * Repeatedly calls a function after a given timeframe has been simulated by
 * the engine.
 */
struct Timer;

typedef void (*TimerCallback)( Timer* timer, double timeDelta, void* context );


bool InitTime();
void DestroyTime();

/**
 * Notifies the module that a timeframe has been simulated.
 *
 * This can in turn trigger timers to call their callback functions.
 */
void UpdateTime( double timeDelta );

/**
 * Simulation time in seconds.
 */
double GetTime();

/**
 * Tries to allocate a new timer object.
 *
 * It's safe to call this inside a timer callback.
 *
 * @param minDelay
 * The implementation guarantees that at least `minDelay` seconds must pass,
 * till the callback is triggered.  See #Timer.
 *
 * @param context
 * Arbitrary user data which is passed through to the callback.
 *
 * @param callback
 * Function which is called whenever the timer has been triggered.
 *
 * @return
 * The newly allocated timer object or `NULL` if
 */
Timer* CreateTimer( double minDelay, void* context, TimerCallback callback );


/**
 * Invalidates a timer object.
 *
 * It's safe to call this inside a timer callback.
 */
void DestroyTimer( Timer* timer );


#endif
