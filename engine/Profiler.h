#ifndef __KONSTRUKT_PROFILER__
#define __KONSTRUKT_PROFILER__
#if defined(KONSTRUKT_PROFILER_ENABLED)

#include "Common.h" // CONCAT

void InitProfiler();
void DestroyProfiler();
void InitGPUProfiler();
void DestroyGPUProfiler();


/**
 *
 */
#define ProfileScope( ... ) \
    static Sample CONCAT(sample_,__LINE__) = _CreateSample({__FILE__,__VA_ARGS__}); \
    _ProfileSampleInScope CONCAT(profileSampleInScope,__LINE__)(&CONCAT(sample_,__LINE__)); \

/**
 *
 */
#define ProfileFunction( ... ) \
    ProfileScope(__func__, __VA_ARGS__)

/**
 *
 */
#define DefineCounter( counter, ... ) \
    static Counter counter = {__FILE__,__VA_ARGS__}

/**
 *
 */
#define InitCounter( counter ) \
    _Profiler.initCounter(&counter)

/**
 *
 */
#define SetCounter( counter, value ) \
    _Profiler.setCounter(&counter, value)

/**
 *
 */
#define IncreaseCounter( counter, value ) \
    _Profiler.increaseCounter(&counter, value)

/**
 *
 */
#define DecreaseCounter( counter, value ) \
    _Profiler.increaseCounter(&counter, -(value))

/**
 * Call when one simulation step has been completed.
 */
#define NotifyProfilerAboutStepCompletion() \
    _Profiler.notifyStepCompletion()

/**
 * Call at the beginning of a new thread.
 *
 * @param name
 * The threads name.
 */
#define NotifyProfilerAboutThreadCreation( name ) \
    _Profiler.notifyThreadCreation(name)


// --- implementation details ---

#include <stdint.h>


enum SampleFlags
{
    GPU_SAMPLE = 1 << 1
};

struct Sample
{
    // Generated fields:
    const char* fileName;
    const char* name;
    int flags;

    // Profiler specific fields:

    /**
     * Id which the profiler may use to identify a sample.
     */
    uint64_t id;
    char module[32];
};

enum CounterType
{
    DEFAULT_COUNTER,
    BYTE_COUNTER
};

struct Counter
{
    // Generated fields:
    const char* fileName;
    const char* name;
    CounterType type;

    // Profiler specific fields:

    /**
     * Id which the profiler may use to identify a counter.
     */
    uint64_t id;
    char path[64];
};

struct Profiler
{
    void (*setup)();
    void (*teardown)();
    void (*setupGPU)();
    void (*teardownGPU)();

    void (*initSample)( Sample* sample );
    void (*beginSample)( Sample* sample );
    void (*endSample)( Sample* sample );

    void (*initCounter)( Counter* counter );
    void (*setCounter)( Counter* counter, int64_t value );
    void (*increaseCounter)( Counter* counter, int64_t value );

    void (*notifyStepCompletion)();
    void (*notifyThreadCreation)( const char* name );
};


extern Profiler _Profiler;

inline Sample _CreateSample( Sample sample )
{
    _Profiler.initSample(&sample);
    return sample;
}

struct _ProfileSampleInScope
{
    Sample* mSample;

    _ProfileSampleInScope( Sample* sample )
    {
        mSample = sample;
        _Profiler.beginSample(sample);
    }

    ~_ProfileSampleInScope()
    {
        _Profiler.endSample(mSample);
    }
};

#else

#define InitProfiler()
#define DestroyProfiler()
#define InitGPUProfiler()
#define DestroyGPUProfiler()
#define ProfileScope(...)
#define ProfileFunction(...)
#define DefineCounter( counter, ... )
#define InitCounter( counter )
#define SetCounter( counter, value )
#define IncreaseCounter( counter, value )
#define DecreaseCounter( counter, value )
#define NotifyProfilerAboutStepCompletion()
#define NotifyProfilerAboutThreadCreation()

#endif // KONSTRUKT_PROFILER_ENABLED
#endif
