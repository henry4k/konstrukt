#ifndef __KONSTRUKT_PROFILER__
#define __KONSTRUKT_PROFILER__
#if defined(KONSTRUKT_PROFILER_ENABLED)

#include <stdint.h>


struct ProfiledFrame
{
    // Generated fields:
    const char* name;
    const char* fileName;

    // Profiler specific fields:

    /**
     * Id which the profiler may use to identify a frame.
     */
    uint64_t id;
    char module[32];
};

/**
 * Is called *once* before the frame is used.
 */
typedef void (*InitProfiledFrameFn)( ProfiledFrame* frame );

typedef void (*BeginProfiledFrameFn)( ProfiledFrame* frame );

typedef void (*EndProfiledFrameFn)();

struct Profiler
{
    void (*setup)();
    void (*teardown)();
    InitProfiledFrameFn  initFrame;
    BeginProfiledFrameFn beginFrame;
    EndProfiledFrameFn   endFrame;
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
#define ProfileScope( name ) \
    static ProfiledFrame CONCAT(profiledFrame,__LINE__) = _CreateProfiledFrame({name,__FILE__}); \
    _BeginProfiledFrame(&CONCAT(profiledFrame,__LINE__)); \
    _EndProfiledFrameOnExit endProfiledFrameOnExit

/**
 *
 */
#define ProfileFunction() ProfileScope(__func__)


// --- implementation details ---

extern InitProfiledFrameFn  _InitProfiledFrame;
extern BeginProfiledFrameFn _BeginProfiledFrame;
extern EndProfiledFrameFn   _EndProfiledFrame;

inline ProfiledFrame _CreateProfiledFrame( ProfiledFrame frame )
{
    _InitProfiledFrame(&frame);
    return frame;
}

struct _EndProfiledFrameOnExit
{
    ~_EndProfiledFrameOnExit()
    {
        _EndProfiledFrame();
    }
};

#else

inline void InitProfiler() {}
inline void DestroyProfiler() {}
#define ProfileScope( name )
#define ProfileFunction()

#endif // KONSTRUKT_PROFILER_ENABLED
#endif
