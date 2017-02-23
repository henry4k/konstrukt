#if defined(KONSTRUKT_PROFILER_ENABLED)
#include "../Profiler.h"

#include "../Warnings.h"
BEGIN_EXTERNAL_CODE
#include <microprofile.h>
END_EXTERNAL_CODE

#include "../Common.h"
#include "MicroProfile.h" // the engine module header, not the library


static void MicroProfile_Setup()
{
    MicroProfileOnThreadCreate("Main");
    MicroProfileSetEnableAllGroups(true);
    MicroProfileSetForceMetaCounters(true);
    MicroProfileStartContextSwitchTrace();
}

static void MicroProfile_Teardown()
{
    //MicroProfileStopContextSwitchTrace();
    //MicroProfileSetForceMetaCounters(false);
    //MicroProfileSetEnableAllGroups(false);
    //MicroProfileOnThreadExit();
    MicroProfileShutdown();
}

static void MicroProfile_InitProfiledFrame( ProfiledFrame* frame )
{
    // TODO: Get module name from basename
    // TODO: Use module name as group
    MicroProfileGetTokenC(&frame->id, "unknown", frame->name, MP_WHEAT, MicroProfileTokenTypeCpu);
}

static void MicroProfile_BeginProfiledFrame( ProfiledFrame* frame )
{
    MicroProfileEnter(frame->id);
}

static void MicroProfile_EndProfiledFrame()
{
    MicroProfileLeave();
}

#if 0
//should be called from newly created threads:
MicroProfileOnThreadCreate(const char* pThreadName);
MicroProfileOnThreadExit();

// call once per frame:
MicroProfileFlip(void* pGpuContext);
#endif

const Profiler MicroProfileProfiler = {MicroProfile_Setup,
                                       MicroProfile_Teardown,
                                       MicroProfile_InitProfiledFrame,
                                       MicroProfile_BeginProfiledFrame,
                                       MicroProfile_EndProfiledFrame};

#endif // KONSTRUKT_PROFILER_ENABLED
