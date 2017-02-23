#if defined(KONSTRUKT_PROFILER_ENABLED)
#include "Profiler.h"

#include <stdlib.h> // NULL
#include <string.h> // strcmp

#include "Common.h"
#include "Config.h"
#include "profiler/Remotery.h"


static void NoOp_Setup() {}
static void NoOp_Teardown() {}
static void NoOp_InitProfiledFrame( ProfiledFrame* frame ) {}
static void NoOp_BeginProfiledFrame( ProfiledFrame* frame ) {}
static void NoOp_EndProfiledFrame() {}

static const Profiler NoOpProfiler = {NoOp_Setup,
                                      NoOp_Teardown,
                                      NoOp_InitProfiledFrame,
                                      NoOp_BeginProfiledFrame,
                                      NoOp_EndProfiledFrame};


InitProfiledFrameFn  _InitProfiledFrame  = NoOp_InitProfiledFrame;
BeginProfiledFrameFn _BeginProfiledFrame = NoOp_BeginProfiledFrame;
EndProfiledFrameFn   _EndProfiledFrame   = NoOp_EndProfiledFrame;

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
    _InitProfiledFrame  = profiler->initFrame;
    _BeginProfiledFrame = profiler->beginFrame;
    _EndProfiledFrame   = profiler->endFrame;
    CurrentProfiler->setup();
}


#endif // KONSTRUKT_PROFILER_ENABLED
