#if defined(KONSTRUKT_PROFILER_ENABLED)
#include "../Warnings.h"
BEGIN_EXTERNAL_CODE
#include <Remotery.h>
END_EXTERNAL_CODE

#include "../Profiler.h"
#include "../Common.h"
#include "Remotery.h" // the engine module header, not the library


static Remotery* remotery = NULL;


static void Remotery_Setup()
{
    Ensure(rmt_CreateGlobalInstance(&remotery) == RMT_ERROR_NONE);
    rmt_SetCurrentThreadName("Main");
}

static void Remotery_SetupGPU()
{
    rmt_BindOpenGL();
}

static void Remotery_TeardownGPU()
{
    rmt_UnbindOpenGL();
}

static void Remotery_Teardown()
{
    rmt_DestroyGlobalInstance(remotery);
    remotery = NULL;
}

static void Remotery_BeginSample( Sample* sample )
{
    if(sample->flags & GPU_SAMPLE)
        _rmt_BeginOpenGLSample(sample->name, (rmtU32*)&sample->id);
    else
        _rmt_BeginCPUSample(sample->name, 0, (rmtU32*)&sample->id);
}

static void Remotery_EndSample( Sample* sample )
{
    if(sample->flags & GPU_SAMPLE)
        rmt_EndOpenGLSample();
    else
        rmt_EndCPUSample();
}

const Profiler RemoteryProfiler = {Remotery_Setup,
                                   Remotery_Teardown,
                                   Remotery_SetupGPU,
                                   Remotery_TeardownGPU,
                                   NULL, // initSample
                                   Remotery_BeginSample,
                                   Remotery_EndSample,
                                   NULL, // initCounter
                                   NULL, // setCounter
                                   NULL, // increaseCounter
                                   NULL}; // completeProfilerStep

#endif // KONSTRUKT_PROFILER_ENABLED
