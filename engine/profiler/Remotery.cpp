#if defined(KONSTRUKT_PROFILER_ENABLED)
#include "../Profiler.h"

#include "../Warnings.h"
BEGIN_EXTERNAL_CODE
#include <Remotery.h>
END_EXTERNAL_CODE

#include "../Common.h"
#include "Remotery.h" // the engine module header, not the library


static Remotery* remotery = NULL;


static void Remotery_Setup()
{
    Ensure(rmt_CreateGlobalInstance(&remotery) == RMT_ERROR_NONE);
    rmt_SetCurrentThreadName("Main");
    rmt_BindOpenGL();
}

static void Remotery_Teardown()
{
    rmt_UnbindOpenGL();
    rmt_DestroyGlobalInstance(remotery);
    remotery = NULL;
}

static void Remotery_InitSample( Sample* sample ) {}

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
                                   Remotery_InitSample,
                                   Remotery_BeginSample,
                                   Remotery_EndSample};

#endif // KONSTRUKT_PROFILER_ENABLED
