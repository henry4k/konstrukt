#if !defined(__WIN32__)
    #include <signal.h>
#endif

#include "Common.h"

const char* Format( const char* format, ... )
{
    static char buffer[512];

    va_list vl;
    va_start(vl, format);
        vsprintf(buffer, format, vl);
    va_end(vl);

    return buffer;
}

void Log( const char* format, ... )
{
    va_list vl;
    va_start(vl, format);
        vprintf(format, vl);
    va_end(vl);
    printf("\n");
}

void Error( const char* format, ... )
{
    fprintf(stderr, "ERROR: ");
    va_list vl;
    va_start(vl, format);
        vfprintf(stderr, format, vl);
    va_end(vl);
    fprintf(stderr, "\n");
}

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
