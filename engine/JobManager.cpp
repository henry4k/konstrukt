#include <tinycthread.h>

#include "Common.h"
#include "Profiler.h"
#include "Array.h"
#include "FixedArray.h"
#include "JobManager.h"


static const int INVALID_JOB_ID = -1;

struct Job
{
    JobStatus status;
    JobConfig config;
};

struct Worker
{
    thrd_t thread;
};

struct WorkerArgument
{
    JobManager* manager;
    int workerId;
};

struct JobManager
{
    JobManagerConfig config;
    mtx_t mutex;

    cnd_t updateCondition; // notifies workers
    bool isStopping; // workers should stop
    Array<Worker> workers;

    FixedArray<Job> jobs;
    Array<cnd_t> jobCompletionConditions; // indices map to job ids
    Array<JobId> jobQueue; // sorted list of queued jobs
};


DefineCounter(JobCounter, "job count");


static int WorkerThreadFn( void* arg );

JobManager* CreateJobManager( JobManagerConfig config )
{
    InitCounter(JobCounter);

    Ensure(config.workerThreads >= 0);

    JobManager* manager = NEW(JobManager);
    manager->config = config;
    Ensure(mtx_init(&manager->mutex, mtx_plain) == thrd_success);
    Ensure(cnd_init(&manager->updateCondition) == thrd_success);
    manager->isStopping = false;
    InitArray(&manager->workers);
    InitFixedArray(&manager->jobs);
    InitArray(&manager->jobCompletionConditions);
    InitArray(&manager->jobQueue);

    AllocateAtEndOfArray(&manager->workers, config.workerThreads);
    REPEAT(manager->workers.length, i)
    {
        Worker* worker = manager->workers.data + i;

        WorkerArgument* arg = NEW(WorkerArgument);
        arg->manager = manager;
        arg->workerId = i;
        Ensure(thrd_create(&worker->thread, WorkerThreadFn, arg) == thrd_success);
    }

    LockJobManager(manager);

    return manager;
}

void DestroyJobManager( JobManager* manager )
{
    manager->isStopping = true;
    cnd_broadcast(&manager->updateCondition);
    UnlockJobManager(manager);

    REPEAT(manager->workers.length, i)
    {
        Worker* worker = manager->workers.data + i;
        Ensure(thrd_join(worker->thread, NULL) == thrd_success);
    }

    REPEAT(manager->jobCompletionConditions.length, i)
        cnd_destroy(manager->jobCompletionConditions.data + i);

    mtx_destroy(&manager->mutex);
    cnd_destroy(&manager->updateCondition);
    DestroyArray(&manager->workers);
    DestroyFixedArray(&manager->jobs);
    DestroyArray(&manager->jobCompletionConditions);
    DestroyArray(&manager->jobQueue);
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

void WaitForJobs( JobManager* manager, const JobId* jobIds, int jobIdCount )
{
    REPEAT(jobIdCount, i)
    {
        const JobId id = jobIds[i];
        while(GetJobStatus(manager, id) != COMPLETED_JOB)
        {
            cnd_t* completionCondition =
                GetArrayElement(&manager->jobCompletionConditions, id);
            Ensure(cnd_wait(completionCondition, &manager->mutex) == thrd_success);
        }
    }
}

JobId CreateJob( JobManager* manager, JobConfig config )
{
    FixedArrayAllocation<Job> allocation = AllocateInFixedArray(&manager->jobs);
    Job* job = allocation.element;
    job->status = QUEUED_JOB;
    job->config = config;
    IncreaseCounter(JobCounter, 1);

    const JobId id = (JobId)allocation.pos;

    // Ensure that a condition variable is available:
    for(int i = manager->jobCompletionConditions.length; i <= id; i++)
    {
        cnd_t* completionCondition =
            AllocateAtEndOfArray(&manager->jobCompletionConditions, 1);
        cnd_init(completionCondition);
    }

    // Insert into job queue:
    InsertInArray(&manager->jobQueue, 0, 1, &id);

    cnd_signal(&manager->updateCondition);
    return id;
}

void RemoveJob( JobManager* manager, JobId jobId )
{
    const Job* job = GetFixedArrayElement(&manager->jobs, jobId);
    Ensure(job->status == COMPLETED_JOB);
    RemoveFromFixedArray(&manager->jobs, jobId);
    DecreaseCounter(JobCounter, 1);
}

JobStatus GetJobStatus( JobManager* manager, JobId jobId )
{
    return GetFixedArrayElement(&manager->jobs, jobId)->status;
}

void* GetJobData( JobManager* manager, JobId jobId )
{
    return GetFixedArrayElement(&manager->jobs, jobId)->config.data;
}


// --- Worker specific ---

// Needs access to manager
static JobId TryToGetQueuedJob( JobManager* manager )
{
    if(manager->jobQueue.length == 0)
        return INVALID_JOB_ID;

    const int queueIndex = manager->jobQueue.length - 1;
    const JobId id = manager->jobQueue.data[queueIndex];

    Job* job = GetFixedArrayElement(&manager->jobs, id);
    Ensure(job->status == QUEUED_JOB);

    RemoveFromArray(&manager->jobQueue, queueIndex, 1);

    job->status = ACTIVE_JOB;
    return id;
}

struct UpdateResult
{
    bool shallStop;
    Job* job;
    JobId jobId;
};

// Needs access to manager
static bool GetUpdate( JobManager* manager, UpdateResult* update )
{
    if(manager->isStopping)
    {
        update->shallStop = true;
        return true;
    }

    const JobId jobId = TryToGetQueuedJob(manager);
    if(jobId != INVALID_JOB_ID)
    {
        update->job = GetFixedArrayElement(&manager->jobs, jobId);
        update->jobId = jobId;
        return true;
    }

    return false;
}

static int WorkerThreadFn( void* arg_ )
{
    WorkerArgument* arg = (WorkerArgument*)arg_;
    JobManager* manager = arg->manager;
    NotifyProfilerAboutThreadCreation(Format("Worker %d", arg->workerId));
    DELETE(arg);

    for(;;)
    {
        LockJobManager(manager);

        UpdateResult update;
        memset(&update, 0, sizeof(update));
        while(!GetUpdate(manager, &update))
            Ensure(cnd_wait(&manager->updateCondition, &manager->mutex) == thrd_success);

        UnlockJobManager(manager);


        if(update.shallStop)
            break;
        else
        {
            update.job->config.function(update.job->config.data);

            LockJobManager(manager);

            Ensure(update.job->status == ACTIVE_JOB);
            update.job->status = COMPLETED_JOB;

            cnd_t* completionCondition =
                GetArrayElement(&manager->jobCompletionConditions, update.jobId);
            cnd_broadcast(completionCondition);

            UnlockJobManager(manager);
        }
    }

    return 0;
}

void Sleep( double seconds )
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

