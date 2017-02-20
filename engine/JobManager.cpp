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

struct Worker
{
    thrd_t thread;
};

struct JobManager
{
    JobManagerConfig config;
    mtx_t mutex;
    cnd_t updateCondition;
    bool isStopping;
    Array<JobType> jobTypes;
    FixedArray<Job> jobs;
    Array<Worker> workers;
};


static int WorkerThreadFn( void* arg );

JobManager* CreateJobManager( JobManagerConfig config )
{
    Ensure(config.workerThreads >= 0);

    JobManager* manager = NEW(JobManager);
    manager->config = config;
    Ensure(mtx_init(&manager->mutex, mtx_plain) == thrd_success);
    Ensure(cnd_init(&manager->updateCondition) == thrd_success);
    manager->isStopping = false;
    InitArray(&manager->jobTypes);
    InitFixedArray(&manager->jobs);
    InitArray(&manager->workers);

    AllocateAtEndOfArray(&manager->workers, config.workerThreads);
    REPEAT(manager->workers.length, i)
    {
        Worker* worker = manager->workers.data + i;
        Ensure(thrd_create(&worker->thread, WorkerThreadFn, manager) == thrd_success);
    }

    return manager;
}

static void Sleep( double seconds );

void DestroyJobManager( JobManager* manager )
{
    LockJobManager(manager);
    manager->isStopping = true;
    cnd_broadcast(&manager->updateCondition);
    UnlockJobManager(manager);

    REPEAT(manager->workers.length, i)
    {
        Worker* worker = manager->workers.data + i;
        Ensure(thrd_join(worker->thread, NULL) == thrd_success);
    }

    mtx_destroy(&manager->mutex);
    cnd_destroy(&manager->updateCondition);
    DestroyArray(&manager->jobTypes);
    DestroyFixedArray(&manager->jobs);
    DestroyArray(&manager->workers);
    DELETE(manager);
}

void LockJobManager( JobManager* manager )
{
    Ensure(mtx_lock(&manager->mutex) == thrd_success);
}

void UnlockJobManager( JobManager* manager )
{
    Ensure(mtx_unlock(&manager->mutex) == thrd_success);
}

static void Sleep( double seconds )
{
    if(seconds > 0)
    {
        timespec duration;
        duration.tv_sec = (int)seconds;
        duration.tv_nsec = (seconds - (int)seconds) * 1e9;

        timespec remaining;
        for(;;)
        {
            const int sleepResult = thrd_sleep(&duration, &remaining);
            Ensure(sleepResult == 0 || sleepResult == -1);

            if(sleepResult == 0)
                break;
            else
                duration = remaining;
        }
    }
    else
    {
        thrd_yield();
    }
}

void RunJobs( double duration )
{
    Sleep(duration);
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


// --- Worker specific ---

// Needs access to manager
static Job* TryToGetQueuedJob( JobManager* manager )
{
    REPEAT(manager->jobs._.length, i)
    {
        FixedArraySlot<Job>* slot = manager->jobs._.data + i;
        Job* job = &slot->element;
        if(slot->inUse && job->status == QUEUED_JOB)
            return job;
    }
    return NULL;
}

static void ExecuteJob( JobManager* manager, Job* job, const JobType* jobType )
{
    jobType->config.function(job->data);

    LockJobManager(manager);
    job->status = COMPLETED_JOB;
    UnlockJobManager(manager);
}

struct UpdateResult
{
    bool shallStop;
    Job* job;
    const JobType* jobType;
};

static bool GetUpdate( JobManager* manager, UpdateResult* update )
{
    if(manager->isStopping)
    {
        update->shallStop = true;
        return true;
    }

    Job* job = TryToGetQueuedJob(manager);
    if(job)
    {
        update->job = job;
        update->jobType = GetArrayElement(&manager->jobTypes, job->typeId);
        return true;
    }

    return false;
}

static int WorkerThreadFn( void* arg )
{
    JobManager* manager = (JobManager*)arg;

    for(;;)
    {
        LockJobManager(manager);

        UpdateResult update;
        memset(&update, 0, sizeof(update));
        while(!GetUpdate(manager, &update))
            Ensure(cnd_wait(&manager->updateCondition, &manager->mutex) == thrd_success);

        if(update.job)
            update.job->status = ACTIVE_JOB;

        UnlockJobManager(manager);


        if(update.shallStop)
            break;
        else
            ExecuteJob(manager, update.job, update.jobType);
    }

    return 0;
}
