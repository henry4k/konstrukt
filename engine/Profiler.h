#ifndef __KONSTRUKT_PROFILER__
#define __KONSTRUKT_PROFILER__
#if defined(KONSTRUKT_PROFILER_ENABLED)

#include <stdint.h>


enum SampleFlags
{
    GPU_SAMPLE = 1 << 1
};

struct Sample
{
    // Generated fields:
    const char* name;
    int flags;

    // Profiler specific fields:

    /**
     * Id which the profiler may use to identify a sample.
     */
    uint64_t id;
};

/**
 * Is called *once* before the sample is used.
 */
typedef void (*InitSampleFn)( Sample* sample );

typedef void (*BeginSampleFn)( Sample* sample );

typedef void (*EndSampleFn)( Sample* sample );

struct Profiler
{
    void (*setup)();
    void (*teardown)();
    InitSampleFn  initSample;
    BeginSampleFn beginSample;
    EndSampleFn   endSample;
};


void InitProfiler();
void DestroyProfiler();

/**
 * Installs a profiler.  Uninstalls the previous one - if there was one.
 *
 * This will call the respective `setup` and `teardown` functions.
 *
 * Passing `NULL` will disable profiling.
 * This essentially points all profiling hooks to no-op functions.
 */
void SetProfiler( const Profiler* profiler );

#define CONCAT_(a, b) a ## b
#define CONCAT(a, b) CONCAT_(a,b)

/**
 *
 */
#define ProfileScope( ... ) \
    static Sample CONCAT(profiledSample,__LINE__) = _CreateSample({__VA_ARGS__}); \
    _ProfileSampleInScope CONCAT(profileSampleInScope,__LINE__)(&CONCAT(profiledSample,__LINE__)); \

/**
 *
 */
#define ProfileFunction( ... ) ProfileScope(__func__, __VA_ARGS__)


// --- implementation details ---

extern InitSampleFn  _InitSample;
extern BeginSampleFn _BeginSample;
extern EndSampleFn   _EndSample;

inline Sample _CreateSample( Sample sample )
{
    _InitSample(&sample);
    return sample;
}

struct _ProfileSampleInScope
{
    Sample* mSample;

    _ProfileSampleInScope( Sample* sample )
    {
        mSample = sample;
        _BeginSample(sample);
    }

    ~_ProfileSampleInScope()
    {
        _EndSample(mSample);
    }
};

#else

inline void InitProfiler() {}
inline void DestroyProfiler() {}
#define ProfileScope( name )
#define ProfileFunction()

#endif // KONSTRUKT_PROFILER_ENABLED
#endif
