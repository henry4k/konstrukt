#include <stddef.h> // NULL
#include <setjmp.h>
#include <string.h> // memset, strncpy
#include <assert.h>


#include "signal.h"
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


static Context* currentContext = NULL;


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

static void signalHandler( int signal )
{
    assert(currentContext);

    // If setAbortInformation has not been called before:
    if(currentContext->errorCode == DUMMY_RUNNER_SUCEEDED)
    {
        setAbortInformation(
            currentContext,
            DUMMY_RUNNER_GENERIC_ERROR,
            dummySignalToAbortReason(signal));
    }

    longjmp(currentContext->jumpBuffer, 1);
}

static int runTest( void* context_, dummyTestFunction fn, const char** abortReason )
{
    Context* context = (Context*)context_;
    memset(context, 0, sizeof(Context));
    context->errorCode = DUMMY_RUNNER_SUCEEDED;

    dummySetSignals(signalHandler);
    currentContext = context;

    const int jumpResult = setjmp(context->jumpBuffer);
    if(jumpResult == 0)
        fn();

    currentContext = NULL;
    dummySetSignals(SIG_DFL);

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
