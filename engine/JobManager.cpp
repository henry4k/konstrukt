#include <tinycthread.h>

#include "Common.h"
#include "Array.h"
#include "FixedArray.h"
#include "JobManager.h"


struct JobType
{
    JobTypeConfig config;
};

struct Job
{
    JobTypeId typeId;
    JobStatus status;
    void* data;
};

struct JobManager
{
    JobManagerConfig config;
    mtx_t mutex;
    Array<JobType> jobTypes;
    FixedArray<Job> jobs;
};


JobManager* CreateJobManager( JobManagerConfig config )
{
    JobManager* manager = NEW(JobManager);
    manager->config = config;
    Ensure(mtx_init(&manager->mutex, mtx_plain) == thrd_success);
    InitArray(&manager->jobTypes);
    InitFixedArray(&manager->jobs);
    return manager;
}

void DestroyJobManager( JobManager* manager )
{
    mtx_destroy(&manager->mutex);
    DestroyArray(&manager->jobTypes);
    DestroyFixedArray(&manager->jobs);
    DELETE(manager);
}

void RunJobs( double duration )
{
}

JobTypeId CreateJobType( JobManager* manager, JobTypeConfig config )
{
    JobType* type = AllocateAtEndOfArray(&manager->jobTypes, 1);
    type->config = config;
    JobTypeId typeId = manager->jobTypes.length - 1;
    return typeId;
}

JobId CreateJob( JobManager* manager, JobTypeId typeId, void* data )
{
    FixedArrayAllocation<Job> allocation = AllocateInFixedArray(&manager->jobs);
    Job* job = allocation.element;
    job->typeId = typeId;
    job->status = QUEUED_JOB;
    job->data = data;
    return (JobId)allocation.pos;
}

void RemoveJob( JobManager* manager, JobId jobId )
{
    RemoveFromFixedArray(&manager->jobs, jobId);
}

JobStatus GetJobStatus( JobManager* manager, JobId jobId )
{
    return GetFixedArrayElement(&manager->jobs, jobId)->status;
}

void* GetJobData( JobManager* manager, JobId jobId )
{
    return GetFixedArrayElement(&manager->jobs, jobId)->data;
}
