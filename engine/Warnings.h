#ifndef __APOAPSIS_WARNINGS__

#if defined(_MSC_VER)
#define BEGIN_EXTERNAL_CODE __pragma(warning(push,1))
#define END_EXTERNAL_CODE   __pragma(warning(pop))
#elif defined(__GNUC__) || defined(__clang__)
#define BEGIN_EXTERNAL_CODE _Pragma("GCC diagnostic push") \
                            _Pragma("GCC diagnostic ignored \"-Wpedantic\"")
                            _Pragma("GCC diagnostic ignored \"-Wunused-variable\"")
#define END_EXTERNAL_CODE   _Pragma("GCC diagnostic pop")
#else
#define BEGIN_EXTERNAL_CODE
#define END_EXTERNAL_CODE
#endif

#endif
