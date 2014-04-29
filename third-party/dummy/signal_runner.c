#include <stddef.h> // NULL
#include <signal.h>
#include <setjmp.h>
#include <string.h> // memset, strncpy
#include <fenv.h> // floating point exceptions
#include <assert.h>

#include "signal_runner.h"


enum
{
    MAX_REASON_LENGTH = 512
};

typedef struct
{
    jmp_buf jumpBuffer;
    int errorCode;
    char abortReason[MAX_REASON_LENGTH];
} Context;

typedef void (*signalFunction)(int);


static Context* currentContext = NULL;


static void signalHandler( int signal );
static void setAbortInformation( Context* context, int errorCode, const char* reason );

static void setSignals( signalFunction fn )
{
    signal(SIGABRT, fn);
    signal(SIGFPE, fn);
    signal(SIGILL, fn);
    signal(SIGSEGV, fn);
}

const char* signalToAbortReason( int signal )
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

static void signalHandler( int signal )
{
    assert(currentContext);

    // If setAbortInformation was not called before:
    if(currentContext->errorCode == DUMMY_RUNNER_SUCEEDED)
    {
        setAbortInformation(
            currentContext,
            DUMMY_RUNNER_GENERIC_ERROR,
            signalToAbortReason(signal));
    }

    longjmp(currentContext->jumpBuffer, 1);
}

static int runTest( void* context_, dummyTestFunction fn, const char** abortReason )
{
    Context* context = (Context*)context_;
    memset(context, 0, sizeof(Context));

    // run function
    context->errorCode = DUMMY_RUNNER_SUCEEDED;
    setSignals(signalHandler);
    currentContext = context;
    const int jumpResult = setjmp(context->jumpBuffer);
    if(jumpResult == 0)
    {
        fn();
    }
    else
    {
        // A signal has been catched: reset signals
        setSignals(signalHandler);
    }
    currentContext = NULL;
    setSignals(SIG_DFL);

    // return results
    if(abortReason)
    {
        if(context->abortReason[0] == '\0')
            *abortReason = NULL;
        else
            *abortReason = context->abortReason;
    }
    return context->errorCode;
}

static void setAbortInformation( Context* context, int errorCode, const char* reason )
{
    context->errorCode = errorCode;
    if(reason)
        strncpy(context->abortReason, reason, MAX_REASON_LENGTH);
}

static void abortTest( void* context_, int errorCode, const char* reason )
{
    Context* context = (Context*)context_;
    setAbortInformation(context, errorCode, reason);
    raise(SIGABRT);
}

const dummyRunner* dummyGetSignalRunner()
{
    static Context context;
    static dummyRunner runner;

    memset(&context, 0, sizeof(Context));

    runner.context = &context;
    runner.run = runTest;
    runner.abort = abortTest;

    return &runner;
}
