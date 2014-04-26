#include <stdio.h>
#include <assert.h>
#include "test_reporter.h"

typedef struct
{
    FILE* file;
} Context;

static void began( void* ctx_ )
{
    const Context* ctx = (Context*)ctx_;
    fprintf(ctx->file, "test count = %d\n",
        dummyGetTestCount()
    );
}

static void completed( void* ctx_ )
{
    const Context* ctx = (Context*)ctx_;
    fprintf(ctx->file, "testing done\n");
}

static void beganTest( void* ctx_ )
{
    const Context* ctx = (Context*)ctx_;
    fprintf(ctx->file, "test number = %d\n",
        dummyGetTestNumber()
    );
    fprintf(ctx->file, "test name = '%s'\n",
        dummyGetTestName()
    );
}

static void completedTest( void* ctx_ )
{
    const Context* ctx = (Context*)ctx_;

    const char* result = NULL;
    switch(dummyGetTestResult())
    {
        case DUMMY_TEST_PASSED:
            result = "passed";
            break;

        case DUMMY_TEST_FAILED:
            result = "failed";
            break;

        case DUMMY_TEST_SKIPPED:
            result = "skipped";
            break;

        default:
            assert(!"Unknown result");
    }
    fprintf(ctx->file, "test result = '%s'\n",
        result
    );
    fprintf(ctx->file, "test abort reason = '%s'\n",
        dummyGetTestAbortReason()
    );

    fprintf(ctx->file, "test todo = %d\n",
        dummyTestIsMarkedAsTodo()
    );
    fprintf(ctx->file, "test todo reason = '%s'\n",
        dummyGetTestTodoReason()
    );

    fprintf(ctx->file, "test done\n");
}

static void log( void* ctx_, const char* message )
{
    const Context* ctx = (Context*)ctx_;
    fprintf(ctx->file, "%s\n",
        message
    );
}

const dummyReporter* dummyGetTestReporter()
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
