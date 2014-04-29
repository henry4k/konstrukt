#include <stddef.h>
#include <exception>

#include "signal_runner.h"
#include "exception_runner.h"


typedef struct
{
    const dummyRunner* signalRunner;
    dummyTestFunction testFunction;
} Context;

class AbortError
{
public:
    int m_ErrorCode;
    const char* m_Reason;

    explicit AbortError( int errorCode, const char* reason ) :
        m_ErrorCode(errorCode),
        m_Reason(reason)
    {
    }
};


static Context* currentContext = NULL;


static void abortTest( void* context_, int errorCode, const char* reason );

static void exceptionWrapper()
{
    try
    {
        currentContext->testFunction();
    }
    catch( AbortError& e )
    {
        currentContext->signalRunner->abort(
            currentContext->signalRunner->context,
            e.m_ErrorCode,
            e.m_Reason);
    }
    catch( std::exception& e )
    {
        currentContext->signalRunner->abort(
            currentContext->signalRunner->context,
            DUMMY_RUNNER_GENERIC_ERROR,
            e.what());
    }
    catch( ... )
    {
        currentContext->signalRunner->abort(
            currentContext->signalRunner->context,
            DUMMY_RUNNER_GENERIC_ERROR,
            "An unknown exception has been thrown.");
    }
}

static int runTest( void* context_, dummyTestFunction fn, const char** abortReason )
{
    Context* context = (Context*)context_;
    context->testFunction = fn;
    currentContext = context;
    const int errorCode = context->signalRunner->run(context->signalRunner->context, exceptionWrapper, abortReason);
    currentContext = NULL;
    context->testFunction = NULL;
    return errorCode;
}

static void abortTest( void* context_, int errorCode, const char* reason )
{
    Context* context = (Context*)context_;
    throw AbortError(errorCode, reason);
}

const dummyRunner* dummyGetExceptionRunner()
{
    static Context context;
    static dummyRunner runner;

    context.signalRunner = dummyGetSignalRunner();
    context.testFunction = NULL;

    runner.context = &context;
    runner.run = runTest;
    runner.abort = abortTest;

    return &runner;
}
