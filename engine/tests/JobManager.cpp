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

InlineTest("single job")
{
    JobManagerConfig managerConfig;
    managerConfig.workerThreads = 3;
    InitJobManager(managerConfig);

    static Work work;
    work.processingTime = 1;
    work.done = false;
    work.destructorCalled = false;
    JobId job = CreateJob({"worker", DoWork, Destructor, &work});

    Require(GetJobStatus(job) == QUEUED_JOB);
    Require(GetJobData(job) == &work);

    WaitForJobs(&job, 1);

    DestroyJobManager();

    Require(work.done);
    Require(work.destructorCalled);
}

InlineTest("single job (fixed)")
{
    JobManagerConfig managerConfig;
    managerConfig.workerThreads = 3;
    InitJobManager(managerConfig);

    Work work;
    work.processingTime = 1;
    work.done = false;
    work.destructorCalled = false;
    JobId job = CreateJob({"worker", DoWork, Destructor, &work});

    Require(GetJobStatus(job) == QUEUED_JOB);
    Require(GetJobData(job) == &work);

    WaitForJobs(&job, 1);

    Require(work.done);
    Require(!work.destructorCalled);

    DestroyJobManager();

    Require(work.destructorCalled);
}

InlineTest("call destructor on job removal")
{
    JobManagerConfig managerConfig;
    managerConfig.workerThreads = 3;
    InitJobManager(managerConfig);

    static Work work;
    work.processingTime = 1;
    work.done = false;
    work.destructorCalled = false;
    JobId job = CreateJob({"worker", DoWork, Destructor, &work});

    Require(GetJobStatus(job) == QUEUED_JOB);
    Require(GetJobData(job) == &work);

    WaitForJobs(&job, 1);

    Require(work.done);
    Require(!work.destructorCalled);

    RemoveJob(job);

    Require(work.destructorCalled);

    DestroyJobManager();
}

InlineTest("queue job while another is running")
{
    JobManagerConfig managerConfig;
    managerConfig.workerThreads = 1;
    InitJobManager(managerConfig);

    // Queue first job:
    Work workA = {3, false, false};
    JobId jobA = CreateJob({"worker A", DoWork, Destructor, &workA});

    UnlockJobManager();
    Sleep(1); // jobA have waited 1 of 3 seconds by now
    LockJobManager();

    Require(GetJobStatus(jobA) == ACTIVE_JOB);
    Require(GetJobData(jobA) == &workA);

    // Queue second job:
    Work workB = {2, false, false};
    JobId jobB = CreateJob({"worker B", DoWork, Destructor, &workB});

    UnlockJobManager();
    Sleep(1); // jobA have waited 2 of 3 seconds by now
    LockJobManager();

    Require(GetJobStatus(jobA) == ACTIVE_JOB);
    Require(GetJobData(jobA) == &workA);

    Require(GetJobStatus(jobB) == QUEUED_JOB);
    Require(GetJobData(jobB) == &workB);

    UnlockJobManager();
    Sleep(2); // jobA should be completed and jobB should be active
    LockJobManager();

    Require(GetJobStatus(jobA) == COMPLETED_JOB);
    Require(GetJobData(jobA) == &workA);
    Require(workA.done);
    Require(!workA.destructorCalled);

    Require(GetJobStatus(jobB) == ACTIVE_JOB);
    Require(GetJobData(jobB) == &workB);

    WaitForJobs(&jobB, 1);

    Require(GetJobStatus(jobB) == COMPLETED_JOB);
    Require(GetJobData(jobB) == &workB);
    Require(workB.done);
    Require(!workB.destructorCalled);

    DestroyJobManager();

    Require(workA.destructorCalled);
    Require(workB.destructorCalled);
}

int main( int argc, char** argv )
{
    InitTests(argc, argv);
    return RunTests();
}
