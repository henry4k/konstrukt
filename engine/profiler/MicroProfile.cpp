#if defined(KONSTRUKT_PROFILER_ENABLED)
#include <string.h> // strlen, strrchr, memcpy
#include <assert.h>
#include <stdint.h> // uint32_t
#include <math.h> // fmodf

#include "../Warnings.h"
BEGIN_EXTERNAL_CODE
#include <microprofile.h>
END_EXTERNAL_CODE

#include "../Profiler.h"
#include "../Common.h"
#include "../Crc32.h"
#include "MicroProfile.h" // the engine module header, not the library


static void MicroProfile_Setup()
{
    MicroProfileInit();
    MicroProfileOnThreadCreate("Main");
    //MicroProfileStartContextSwitchTrace();
}

static void MicroProfile_Teardown()
{
    //MicroProfileStopContextSwitchTrace();
    MicroProfileShutdown();
}

// Shamelessly stolen from NanoVG (https://github.com/memononen/nanovg)
static float Clamp(float a, float mn, float mx) { return a < mn ? mn : (a > mx ? mx : a); }

// Shamelessly stolen from NanoVG (https://github.com/memononen/nanovg)
static float Hue(float h, float m1, float m2)
{
    if (h < 0) h += 1;
    if (h > 1) h -= 1;
    if (h < 1.0f/6.0f)
        return m1 + (m2 - m1) * h * 6.0f;
    else if (h < 3.0f/6.0f)
        return m2;
    else if (h < 4.0f/6.0f)
        return m1 + (m2 - m1) * (2.0f/3.0f - h) * 6.0f;
    return m1;
}

union rgba8_helper
{
    uint8_t components[4];
    uint32_t combination;
};

// Shamelessly stolen from NanoVG (https://github.com/memononen/nanovg)
static uint32_t HSLtoRGB(float h, float s, float l)
{
    float m1, m2;
    h = fmodf(h, 1.0f);
    if (h < 0.0f) h += 1.0f;
    s = Clamp(s, 0.0f, 1.0f);
    l = Clamp(l, 0.0f, 1.0f);
    m2 = l <= 0.5f ? (l * (1 + s)) : (l + s - l * s);
    m1 = 2 * l - m2;
    float r = Clamp(Hue(h + 1.0f/3.0f, m1, m2), 0.0f, 1.0f);
    float g = Clamp(Hue(h, m1, m2), 0.0f, 1.0f);
    float b = Clamp(Hue(h - 1.0f/3.0f, m1, m2), 0.0f, 1.0f);
    rgba8_helper rgba;
    rgba.components[0] = (uint8_t)(r * 255.0f);
    rgba.components[1] = (uint8_t)(g * 255.0f);
    rgba.components[2] = (uint8_t)(b * 255.0f);
    rgba.components[3] = 0;
    return rgba.combination;
}

static const char* GetBaseName( const char* path )
{
    int i = strlen(path)-1;
    for(; i >= 0; i--)
        if(path[i] == '/' ||
           path[i] == '\\')
            return &path[i+1];
    return path;
}

static void GetModuleName( const char* path, char* out, size_t outSize )
{
    const char* moduleStart = GetBaseName(path);
    const char* moduleEnd = strrchr(moduleStart, '.');
    assert(moduleEnd);
    const size_t length = moduleEnd - moduleStart;
    assert(length+1 < outSize);
    memcpy(out, moduleStart, length);
    out[length] = '\0';
}

static void MicroProfile_InitSample( Sample* sample )
{
    GetModuleName(sample->fileName, sample->module, FIELD_SIZE(Sample, module));

    // Set color:
    const double hue = (double)CalcCrc32ForString(sample->module) / (double)UINT32_MAX;
    const uint32_t color = HSLtoRGB((float)hue, 1.0f, 0.5f);

    MicroProfileRegisterGroup(sample->module, "engine", color);

    sample->id = MICROPROFILE_INVALID_TOKEN;
    MicroProfileGetTokenC(&sample->id, sample->module, sample->name, color, MicroProfileTokenTypeCpu);
}

static void MicroProfile_BeginSample( Sample* sample )
{
    if(sample->flags & GPU_SAMPLE)
        return;
    else
        MicroProfileEnter(sample->id);
}

static void MicroProfile_EndSample( Sample* sample )
{
    if(sample->flags & GPU_SAMPLE)
        return;
    else
        MicroProfileLeave();
}

static void MicroProfile_InitCounter( Counter* counter )
{
    // Has already been initialized.
    if(counter->path[0] != '\0')
        return;

    char module[32];
    GetModuleName(counter->fileName, module, sizeof(module));
    FormatBuffer(counter->path, FIELD_SIZE(Counter, path), "%s/%s", module, counter->name);

    counter->id = MicroProfileGetCounterToken(counter->path);

    int format;
    switch(counter->type)
    {
        case DEFAULT_COUNTER:
            format = MICROPROFILE_COUNTER_FORMAT_DEFAULT;
            break;

        case BYTE_COUNTER:
            format = MICROPROFILE_COUNTER_FORMAT_BYTES;
            break;

        default:
            format = 0;
            FatalError("Unknown format.");
    }
    MicroProfileCounterConfig(counter->path, format, 0, MICROPROFILE_COUNTER_FLAG_DETAILED);
}

static void MicroProfile_SetCounter( Counter* counter, int64_t value )
{
    MicroProfileCounterSet(counter->id, value);
}

static void MicroProfile_IncreaseCounter( Counter* counter, int64_t value )
{
    MicroProfileCounterAdd(counter->id, value);
}

static void MicroProfile_NotifyStepCompletion()
{
    MicroProfileFlip(NULL);
}

static void MicroProfile_NotifyThreadCreation( const char* name )
{
    MicroProfileOnThreadCreate(name);
}

const Profiler MicroProfileProfiler = {MicroProfile_Setup,
                                       MicroProfile_Teardown,
                                       NULL,
                                       NULL,
                                       MicroProfile_InitSample,
                                       MicroProfile_BeginSample,
                                       MicroProfile_EndSample,
                                       MicroProfile_InitCounter,
                                       MicroProfile_SetCounter,
                                       MicroProfile_IncreaseCounter,
                                       MicroProfile_NotifyStepCompletion,
                                       MicroProfile_NotifyThreadCreation};

#endif // KONSTRUKT_PROFILER_ENABLED
