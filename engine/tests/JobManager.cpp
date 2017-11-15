#include <tinycthread.h>

#include "../JobManager.h"
#include "../Common.h"
#include "TestTools.h"
#include <dummy/inline.hpp>

#define InlineTest DUMMY_INLINE_TEST

static double PROCESSING_TIME_FACTOR = 1.0;

struct Work
{
    int processingTime;
    bool done;
};

static void DoWork( void* data )
{
    Work* work = (Work*)data;
    LogNotice("working ...");
    Sleep((double)work->processingTime * PROCESSING_TIME_FACTOR);
    LogNotice("work done");
    work->done = true;
}

InlineTest("single job", dummySignalSandbox)
{
    JobManagerConfig managerConfig;
    managerConfig.workerThreads = 3;
    JobManager* manager = CreateJobManager(managerConfig);

    static Work work;
    work.processingTime = 1;
    work.done = false;
    JobId job = CreateJob(manager, {"worker", DoWork, &work});

    Require(GetJobStatus(manager, job) == QUEUED_JOB);
    Require(GetJobData(manager, job) == &work);

    WaitForJobs(manager, &job, 1);

    DestroyJobManager(manager);

    Require(work.done);
}

InlineTest("single job (fixed)", dummySignalSandbox)
{
    JobManagerConfig managerConfig;
    managerConfig.workerThreads = 3;
    JobManager* manager = CreateJobManager(managerConfig);

    Work work;
    work.processingTime = 1;
    work.done = false;
    JobId job = CreateJob(manager, {"worker", DoWork, &work});

    Require(GetJobStatus(manager, job) == QUEUED_JOB);
    Require(GetJobData(manager, job) == &work);

    WaitForJobs(manager, &job, 1);

    Require(work.done);

    DestroyJobManager(manager);
}

InlineTest("queue job while another is running", dummySignalSandbox)
{
    JobManagerConfig managerConfig;
    managerConfig.workerThreads = 1;
    JobManager* manager = CreateJobManager(managerConfig);

    // Queue first job:
    Work workA = {3, false};
    JobId jobA = CreateJob(manager, {"worker A", DoWork, &workA});

    UnlockJobManager(manager);
    Sleep(1); // jobA have waited 1 of 3 seconds by now
    LockJobManager(manager);

    Require(GetJobStatus(manager, jobA) == ACTIVE_JOB);
    Require(GetJobData(manager, jobA) == &workA);

    // Queue second job:
    Work workB = {1, false};
    JobId jobB = CreateJob(manager, {"worker B", DoWork, &workB});

    UnlockJobManager(manager);
    Sleep(1); // jobA have waited 2 of 3 seconds by now
    LockJobManager(manager);

    Require(GetJobStatus(manager, jobA) == ACTIVE_JOB);
    Require(GetJobData(manager, jobA) == &workA);

    Require(GetJobStatus(manager, jobB) == QUEUED_JOB);
    Require(GetJobData(manager, jobB) == &workB);

    UnlockJobManager(manager);
    Sleep(2); // jobA should be completed and jobB should be active
    LockJobManager(manager);

    Require(GetJobStatus(manager, jobA) == COMPLETED_JOB);
    Require(GetJobData(manager, jobA) == &workA);
    Require(workA.done);

    Require(GetJobStatus(manager, jobB) == ACTIVE_JOB);
    Require(GetJobData(manager, jobB) == &workB);

    WaitForJobs(manager, &jobB, 1);

    Require(GetJobStatus(manager, jobB) == COMPLETED_JOB);
    Require(GetJobData(manager, jobB) == &workB);
    Require(workB.done);

    DestroyJobManager(manager);
}

/*
InlineTest("queue many jobs at once", dummySignalSandbox) // is more of a benchmark
{
    static const int JOB_COUNT = 100;

    JobManagerConfig managerConfig;
    managerConfig.workerThreads = 3;
    JobManager* manager = CreateJobManager(managerConfig);

    // Queue all jobs:
    Work work[JOB_COUNT];
    JobId jobs[JOB_COUNT];
    REPEAT(JOB_COUNT, i)
    {
        work[i].processingTime = 1;
        work[i].done = false;
        jobs[i] = CreateJob(manager, {"worker", DoWork, &work[i]});

        Require(GetJobStatus(manager, jobs[i]) == QUEUED_JOB);
        Require(GetJobData(manager, jobs[i]) == &work[i]);
    }

    WaitForJobs(manager, jobs, JOB_COUNT);

    REPEAT(JOB_COUNT, i)
    {
        Require(work[i].done);
        Require(GetJobStatus(manager, jobs[i]) == COMPLETED_JOB);
        Require(GetJobData(manager, jobs[i]) == &work[i]);
    }

    DestroyJobManager(manager);
}
*/

int main( int argc, char** argv )
{
    InitTests(argc, argv);
    dummyAddInlineTests();
    return RunTests();
}
