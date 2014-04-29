#include <stddef.h> // NULL
#include <exception>

#include "signal.h"
#include "exception_runner.h"


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

static void abortTest( void* context_, int errorCode, const char* reason )
{
    throw AbortError(errorCode, reason);
}

static void signalHandler( int signal )
{
    throw AbortError(DUMMY_RUNNER_GENERIC_ERROR, dummySignalToAbortReason(signal));
}

static int runTest( void* context_, dummyTestFunction fn, const char** abortReason )
{
    const char* reason = NULL;
    int exitCode = DUMMY_RUNNER_SUCEEDED;
    dummySetSignals(signalHandler);

    try
    {
        fn();
    }
    catch( AbortError& e )
    {
        reason = e.m_Reason;
        exitCode = e.m_ErrorCode;
    }
    catch( std::exception& e )
    {
        reason = e.what();
        exitCode = DUMMY_RUNNER_GENERIC_ERROR;
    }
    catch( ... )
    {
        reason = "An unknown exception has been thrown.";
        exitCode = DUMMY_RUNNER_GENERIC_ERROR;
    }

    dummySetSignals(SIG_DFL);
    if(abortReason)
        *abortReason = reason;
    return exitCode;
}

const dummyRunner* dummyGetExceptionRunner()
{
    static dummyRunner runner;

    runner.context = NULL;
    runner.run = runTest;
    runner.abort = abortTest;

    return &runner;
}
