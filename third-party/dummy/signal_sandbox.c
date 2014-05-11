#include <stddef.h> // NULL
#include <setjmp.h>
#include <string.h> // memset, strncpy
#include <assert.h>


#include "signal.h"
#include "signal_sandbox.h"


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

static void abortHandler( void* context_, int errorCode, const char* reason )
{
    Context* context = (Context*)context_;
    setAbortInformation(context, errorCode, reason);
    raise(SIGABRT);
}

static void signalHandler( int signal )
{
    assert(currentContext);

    // If setAbortInformation has not been called before:
    if(currentContext->errorCode == DUMMY_SANDBOX_SUCCEEDED)
    {
        setAbortInformation(
            currentContext,
            DUMMY_SANDBOX_GENERIC_ERROR,
            dummySignalToAbortReason(signal));
    }

    longjmp(currentContext->jumpBuffer, 1);
}

int dummySignalSandbox( dummySandboxableFunction fn, const char** abortReason )
{
    Context context;
    memset(&context, 0, sizeof(Context));
    context.errorCode = DUMMY_SANDBOX_SUCCEEDED;

    dummyPushAbortHandler(abortHandler, &context);
    dummyPushSignalHandler(signalHandler);
    Context* previousContext = currentContext;
    currentContext = &context;

    const int jumpResult = setjmp(context.jumpBuffer);
    if(jumpResult == 0)
        fn();

    currentContext = previousContext;
    dummyPopSignalHandler();
    dummyPopAbortHandler();

    // return results
    if(abortReason)
    {
        if(context.abortReason[0] == '\0')
            *abortReason = NULL;
        else
            *abortReason = context.abortReason;
    }
    return context.errorCode;
}
