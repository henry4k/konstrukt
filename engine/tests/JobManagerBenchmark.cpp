#include "../JobManager.h"
#include "../Common.h"
#include "TestTools.h"


static double PROCESSING_TIME_FACTOR = 1.0;

struct Work
{
    int processingTime;
    bool done;
    bool destructorCalled;
};

static void DoWork( void* data )
{
    Work* work = (Work*)data;
    LogNotice("working ...");
    Sleep((double)work->processingTime * PROCESSING_TIME_FACTOR);
    LogNotice("work done");
    work->done = true;
}

static void Destructor( void* data )
{
    Work* work = (Work*)data;
    Require(!work->destructorCalled);
    work->destructorCalled = true;
}

InlineTest("queue many jobs at once")
{
    static const int JOB_COUNT = 100;

    JobManagerConfig managerConfig;
    managerConfig.workerThreads = 3;
    InitJobManager(managerConfig);

    // Queue all jobs:
    Work work[JOB_COUNT];
    JobId jobs[JOB_COUNT];
    REPEAT(JOB_COUNT, i)
    {
        work[i].processingTime = 1;
        work[i].done = false;
        jobs[i] = CreateJob({"worker", DoWork, Destructor, &work[i]});

        Require(GetJobStatus(jobs[i]) == QUEUED_JOB);
        Require(GetJobData(jobs[i]) == &work[i]);
    }

    WaitForJobs(jobs, JOB_COUNT);
    REPEAT(JOB_COUNT, i)
    {
        Require(work[i].done);
        Require(GetJobStatus(jobs[i]) == COMPLETED_JOB);
        Require(GetJobData(jobs[i]) == &work[i]);
    }

    DestroyJobManager();
}

int main( int argc, char** argv )
{
    InitTests(argc, argv);
    return RunTests();
}
