#ifndef __APOAPSIS_TIME__
#define __APOAPSIS_TIME__


bool InitTime();
void DestroyTime();

/**
 * Simulation time in seconds.
 *
 * TODO: Maybe separate time since application start and simulation time.
 */
double GetTime();


#endif
