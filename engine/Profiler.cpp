#if defined(KONSTRUKT_PROFILER_ENABLED)
#include "Profiler.h"

#include <stdlib.h> // NULL
#include <string.h> // strcmp

#include "Common.h"
#include "Config.h"
#include "profiler/Remotery.h"


static void NoOp_Setup() {}
static void NoOp_Teardown() {}
static void NoOp_InitSample( Sample* sample ) {}
static void NoOp_BeginSample( Sample* sample ) {}
static void NoOp_EndSample( Sample* sample ) {}

static const Profiler NoOpProfiler = {NoOp_Setup,
                                      NoOp_Teardown,
                                      NoOp_InitSample,
                                      NoOp_BeginSample,
                                      NoOp_EndSample};


InitSampleFn  _InitSample  = NoOp_InitSample;
BeginSampleFn _BeginSample = NoOp_BeginSample;
EndSampleFn   _EndSample   = NoOp_EndSample;

static const Profiler* CurrentProfiler = &NoOpProfiler;


void InitProfiler()
{
    const char* profilerName = GetConfigString("debug.profiler", "");
    if(strcmp(profilerName, "") == 0)
        SetProfiler(&NoOpProfiler);
    else if(strcmp(profilerName, "remotery") == 0)
        SetProfiler(&RemoteryProfiler);
    else
        FatalError("Unknown profiler '%s'", profilerName);
}

void DestroyProfiler()
{
    SetProfiler(&NoOpProfiler);
}

void SetProfiler( const Profiler* profiler )
{
    if(!profiler)
        profiler = &NoOpProfiler;

    CurrentProfiler->teardown();
    CurrentProfiler = profiler;
    _InitSample  = profiler->initSample;
    _BeginSample = profiler->beginSample;
    _EndSample   = profiler->endSample;
    CurrentProfiler->setup();
}

#endif // KONSTRUKT_PROFILER_ENABLED
