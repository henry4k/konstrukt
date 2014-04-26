#include <stdio.h>
#include <assert.h>
#include "tap_reporter.h"

typedef struct
{
    FILE* file;
} Context;

static void began( void* ctx_ )
{
    const Context* ctx = (Context*)ctx_;
    fprintf(ctx->file, "1..%d\n", dummyGetTestCount());
}

static void completed( void* ctx_ )
{
}

static void beganTest( void* ctx_ )
{
}

static const char* dummyTestResultToTAPString( dummyTestResult result )
{
    switch(result)
    {
        case DUMMY_TEST_PASSED:
            return "ok";
        case DUMMY_TEST_FAILED:
            return "not ok";
        case DUMMY_TEST_SKIPPED:
            return "ok";
        default:
            assert(!"Unknown result");
    }
}

static void completedTest( void* ctx_ )
{
    const Context* ctx = (Context*)ctx_;

    dummyTestResult result = dummyGetTestResult();
    const char* resultString = dummyTestResultToTAPString(result);
    const char* abortReason = dummyGetTestAbortReason();

    if(result == DUMMY_TEST_FAILED && abortReason)
    {
        fprintf(ctx->file, "# %s\n", abortReason);
    }

    fprintf(ctx->file, "%s %d %s",
        resultString,
        dummyGetTestNumber()+1,
        dummyGetTestName()
    );

    if(dummyTestIsMarkedAsTodo())
    {
        fprintf(ctx->file, " # TODO");
        const char* todoReason = dummyGetTestTodoReason();
        if(todoReason)
            fprintf(ctx->file, " %s", todoReason);
    }
    else if(result == DUMMY_TEST_SKIPPED)
    {
        fprintf(ctx->file, " # SKIP");
        if(abortReason)
            fprintf(ctx->file, " %s", abortReason);
    };

    fprintf(ctx->file, "\n");
}

static void log( void* ctx_, const char* message )
{
    const Context* ctx = (Context*)ctx_;
    fprintf(ctx->file, "# %s\n", message);
}

const dummyReporter* dummyGetTAPReporter()
{
    static Context ctx;
    static dummyReporter reporter;

    ctx.file = stdout;

    reporter.context = &ctx;
    reporter.began = began;
    reporter.completed = completed;
    reporter.beganTest = beganTest;
    reporter.completedTest = completedTest;
    reporter.log = log;

    return &reporter;
}
