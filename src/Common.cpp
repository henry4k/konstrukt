#if !defined(__WIN32__)
    #include <signal.h>
#endif

#include "Common.h"

void FatalError( const char* format, ... )
{
    fprintf(stderr, "FATAL ERROR: ");
    va_list vl;
    va_start(vl, format);
        vfprintf(stderr, format, vl);
    va_end(vl);
    fprintf(stderr, "\n");

#if defined(__WIN32__)
    abort();
#else
    raise(SIGTRAP);
#endif
}
