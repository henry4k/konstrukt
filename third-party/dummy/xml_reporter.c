#include <stdio.h>
#include <assert.h>
#include "xml_reporter.h"

typedef struct
{
    FILE* file;
    const char* tab;
    const char* newline;
} Context;

static void began( void* ctx_ )
{
    const Context* ctx = (Context*)ctx_;
    fprintf(ctx->file, "<testsuite tests=\"%d\">%s",
        dummyGetTestCount(),
        ctx->newline
    );
}

static void completed( void* ctx_ )
{
    const Context* ctx = (Context*)ctx_;
    fprintf(ctx->file, "</testsuite>%s",
        ctx->newline
    );
}

static void beganTest( void* ctx_ )
{
    const Context* ctx = (Context*)ctx_;
    fprintf(ctx->file, "%s<testcase name=\"%s\">%s",
        ctx->tab,
        dummyGetTestName(),
        ctx->newline
    );
}

static void completedTest( void* ctx_ )
{
    const Context* ctx = (Context*)ctx_;

    const char* abortReason = dummyGetTestAbortReason();

    dummyTestResult result = dummyGetTestResult();
    switch(result)
    {
        case DUMMY_TEST_PASSED:
            break;

        case DUMMY_TEST_FAILED:
            if(abortReason)
            {
                fprintf(ctx->file, "%s%s<failure>%s</failure>%s",
                    ctx->tab,
                    ctx->tab,
                    abortReason,
                    ctx->newline
                );
            }
            else
            {
                fprintf(ctx->file, "%s%s<failure/>%s",
                    ctx->tab,
                    ctx->tab,
                    ctx->newline
                );
            }
            break;

        case DUMMY_TEST_SKIPPED:
            if(abortReason)
            {
                fprintf(ctx->file, "%s%s<skipped>%s</skipped>%s",
                    ctx->tab,
                    ctx->tab,
                    abortReason,
                    ctx->newline
                );
            }
            else
            {
                fprintf(ctx->file, "%s%s<skipped/>%s",
                    ctx->tab,
                    ctx->tab,
                    ctx->newline
                );
            }
            break;

        default:
            assert(!"Unknown result");
    }

    fprintf(ctx->file, "%s</testcase>%s",
        ctx->tab,
        ctx->newline
    );
}

static void log( void* ctx_, const char* message )
{
    const Context* ctx = (Context*)ctx_;
    fprintf(ctx->file, "%s%s<system-out>%s</system-out>%s",
        ctx->tab,
        ctx->tab,
        message,
        ctx->newline
    );
}

const dummyReporter* dummyGetXMLReporter( FILE* file, const char* tab, const char* newline )
{
    static Context ctx;
    static dummyReporter reporter;

    ctx.file = file;
    ctx.tab = tab;
    ctx.newline = newline;

    reporter.context = &ctx;
    reporter.began = began;
    reporter.completed = completed;
    reporter.beganTest = beganTest;
    reporter.completedTest = completedTest;
    reporter.log = log;

    return &reporter;
}
