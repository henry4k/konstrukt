#include <tinycthread.h>

#include "../JobManager.h"
#include "../Common.h"
#include "TestTools.h"


struct Frobnicatable
{
    int processingTime;
    bool done;
};

void Frobnicate( void* data )
{
    Frobnicatable* f = (Frobnicatable*)data;

    // Wait `f->processingTime` seconds:
    timespec duration;
    duration.tv_sec = f->processingTime;
    duration.tv_nsec = 0;
    LogNotice("frobnicating ...");
    thrd_sleep(&duration, NULL);
    LogNotice("frobnicating done");

    f->done = true;
}

void Test()
{
    JobManagerConfig managerConfig;
    managerConfig.workerThreads = 4;
    JobManager* manager = CreateJobManager(managerConfig);

    JobTypeConfig typeConfig;
    typeConfig.name = "frobnicator";
    typeConfig.function = Frobnicate;
    JobTypeId type = CreateJobType(manager, typeConfig);

    static Frobnicatable data;
    data.processingTime = 3;
    data.done = false;
    JobId job = CreateJob(manager, type, &data);

    Require(GetJobStatus(manager, job) == QUEUED_JOB);
    Require(GetJobData(manager, job) == &data);

    RunJobs(2);

    DestroyJobManager(manager);

    Require(data.done);
}


int main( int argc, char** argv )
{
    InitTests(argc, argv);

    AddTest("Test", Test);

    return RunTests();
}
