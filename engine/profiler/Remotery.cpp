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
    rmt_DestroyGlobalInstance(remotery);
    rmt_UnbindOpenGL();
    remotery = NULL;
}

static void Remotery_InitProfiledFrame( ProfiledFrame* frame ) {}

static void Remotery_BeginProfiledFrame( ProfiledFrame* frame )
{
    _rmt_BeginCPUSample(frame->name, 0, (rmtU32*)&frame->id);
}

static void Remotery_EndProfiledFrame()
{
    rmt_EndCPUSample();
}

const Profiler RemoteryProfiler = {Remotery_Setup,
                                   Remotery_Teardown,
                                   Remotery_InitProfiledFrame,
                                   Remotery_BeginProfiledFrame,
                                   Remotery_EndProfiledFrame};

#endif // KONSTRUKT_PROFILER_ENABLED
