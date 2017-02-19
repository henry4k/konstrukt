#include "../JobManager.h"
#include "TestTools.h"


struct Frobnicatable
{
    float processingTime;
};

void Frobnicate( void* data )
{
    Frobnicatable* f = (Frobnicatable*)data;
    // TODO: Wait `f->processingTime` seconds.
}

void Test()
{
    JobManagerConfig managerConfig;
    managerConfig.workerThreads = -1;
    JobManager* manager = CreateJobManager(managerConfig);

    JobTypeConfig typeConfig;
    typeConfig.name = "frobnicator";
    typeConfig.function = Frobnicate;
    JobTypeId type = CreateJobType(manager, typeConfig);

    static Frobnicatable data;
    data.processingTime = 2;
    JobId job = CreateJob(manager, type, &data);

    Require(GetJobStatus(manager, job) == QUEUED_JOB);
    Require(GetJobData(manager, job) == &data);

    RemoveJob(manager, job);

    DestroyJobManager(manager);
}


int main( int argc, char** argv )
{
    InitTests(argc, argv);

    AddTest("Test", Test);

    return RunTests();
}
