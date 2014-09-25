#ifndef __APOAPSIS_GAME__
#define __APOAPSIS_GAME__

static const int SIMULATION_FREQUENCY = 20;
static const int MAX_FRAME_FREQUENCY = 120;

bool InitGame( const int argc, char** argv );
void RunGame();
float CurrentTimeFrame();

#endif
