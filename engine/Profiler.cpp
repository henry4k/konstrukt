#if defined(KONSTRUKT_PROFILER_ENABLED)
#include "Profiler.h"

#include <stdlib.h> // NULL
#include <string.h> // strcmp
#include <assert.h>

#include "Common.h"
#include "Config.h"
#include "profiler/Remotery.h"
#include "profiler/MicroProfile.h"


static void NoOp_Setup() {}
static void NoOp_Teardown() {}
static void NoOp_SetupGPU() {}
static void NoOp_TeardownGPU() {}
static void NoOp_InitSample( Sample* sample ) {}
static void NoOp_BeginSample( Sample* sample ) {}
static void NoOp_EndSample( Sample* sample ) {}
static void NoOp_InitCounter( Counter* counter ) {}
static void NoOp_SetCounter( Counter* counter, int64_t value ) {}
static void NoOp_IncreaseCounter( Counter* counter, int64_t value ) {}
static void NoOp_NotifyProfilerAboutStepCompletion() {}
static void NoOp_NotifyProfilerAboutThreadCreation( const char* name ) {}

static const Profiler NoOpProfiler = {NoOp_Setup,
                                      NoOp_Teardown,
                                      NoOp_SetupGPU,
                                      NoOp_TeardownGPU,
                                      NoOp_InitSample,
                                      NoOp_BeginSample,
                                      NoOp_EndSample,
                                      NoOp_InitCounter,
                                      NoOp_SetCounter,
                                      NoOp_IncreaseCounter,
                                      NoOp_NotifyProfilerAboutStepCompletion,
                                      NoOp_NotifyProfilerAboutThreadCreation};

Profiler _Profiler = NoOpProfiler;


void InitProfiler()
{
    const char* profilerName = GetConfigString("debug.profiler", "");
    if(strcmp(profilerName, "") == 0)
        _Profiler = NoOpProfiler;
    else if(strcmp(profilerName, "remotery") == 0)
        _Profiler = RemoteryProfiler;
    else if(strcmp(profilerName, "microprofile") == 0)
        _Profiler = MicroProfileProfiler;
    else
        FatalError("Unknown profiler '%s'", profilerName);

#define DEFAULT_TO_NOOP( field ) \
    if(!_Profiler.field) _Profiler.field = NoOpProfiler.field;
    DEFAULT_TO_NOOP(setup)
    DEFAULT_TO_NOOP(teardown)
    DEFAULT_TO_NOOP(setupGPU)
    DEFAULT_TO_NOOP(teardownGPU)
    DEFAULT_TO_NOOP(initSample)
    DEFAULT_TO_NOOP(beginSample)
    DEFAULT_TO_NOOP(endSample)
    DEFAULT_TO_NOOP(initCounter)
    DEFAULT_TO_NOOP(setCounter)
    DEFAULT_TO_NOOP(increaseCounter)
    DEFAULT_TO_NOOP(notifyStepCompletion)
    DEFAULT_TO_NOOP(notifyThreadCreation)
#undef DEFAULT_TO_NOOP

    _Profiler.setup();
}

void DestroyProfiler()
{
    _Profiler.teardown();
}

void InitGPUProfiler()
{
    _Profiler.setupGPU();
}

void DestroyGPUProfiler()
{
    _Profiler.teardownGPU();
}

#endif // KONSTRUKT_PROFILER_ENABLED
