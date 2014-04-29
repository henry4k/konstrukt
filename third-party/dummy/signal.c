#include "signal.h"

void dummySetSignals( dummySignalFunction fn )
{
    signal(SIGABRT, fn);
    signal(SIGFPE, fn);
    signal(SIGILL, fn);
    signal(SIGSEGV, fn);
}

const char* dummySignalToAbortReason( int signal )
{
    switch(signal)
    {
        case SIGABRT:
            return "SIGABRT: Abnormal termination";

        case SIGILL:
            return "SIGILL: Illegal instruction";

        case SIGSEGV:
            return "SIGSEGV: Segmentation violation";

        case SIGFPE:
            //const int exception = fetestexcept(FE_ALL_EXCEPT);
            //if(exception & FE_DIVBYZERO) puts("FE_DIVBYZERO");
            //if(exception & FE_INEXACT)   puts("FE_INEXACT");
            //if(exception & FE_INVALID)   puts("FE_INVALID");
            //if(exception & FE_OVERFLOW)  puts("FE_OVERFLOW");
            //if(exception & FE_UNDERFLOW) puts("FE_UNDERFLOW");
            return "SIGFPE: Floating point exception";

        default:
            return "An unknown signal has been raised.";
    }
}