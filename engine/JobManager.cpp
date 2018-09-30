#include <assert.h>
#include <tinycthread.h>

#include "Common.h"
#include "Profiler.h"
#include "Array.h"
#include "FixedArray.h"
#include "JobManager.h"


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
    int workerId;
};

static struct
{
    JobManagerConfig config;
    mtx_t mutex;

    cnd_t updateCondition; // notifies workers
    bool isStopping; // workers should stop
    Array<Worker> workers;

    FixedArray<Job> jobs;
    Array<cnd_t> jobCompletionConditions; // indices map to job ids
    Array<JobId> jobQueue; // sorted list of queued jobs
} JobManager;


DefineCounter(JobCounter, "job count");


static int WorkerThreadFn( void* arg );

void InitJobManager( JobManagerConfig config )
{
    assert(InSerialPhase());

    InitCounter(JobCounter);

    Ensure(config.workerThreads >= 0);

    memset(&JobManager, 0, sizeof(JobManager));

    JobManager.config = config;
    Ensure(mtx_init(&JobManager.mutex, mtx_plain) == thrd_success);
    Ensure(cnd_init(&JobManager.updateCondition) == thrd_success);
    JobManager.isStopping = false;
    InitArray(&JobManager.workers);
    InitFixedArray(&JobManager.jobs);
    InitArray(&JobManager.jobCompletionConditions);
    InitArray(&JobManager.jobQueue);

    AllocateAtEndOfArray(&JobManager.workers, config.workerThreads);
    REPEAT(JobManager.workers.length, i)
    {
        Worker* worker = JobManager.workers.data + i;

        WorkerArgument* arg = NEW(WorkerArgument);
        arg->workerId = i;
        Ensure(thrd_create(&worker->thread, WorkerThreadFn, arg) == thrd_success);
    }

    LockJobManager();
}

void DestroyJobManager()
{
    assert(InSerialPhase());

    JobManager.isStopping = true;
    cnd_broadcast(&JobManager.updateCondition);
    UnlockJobManager();

    REPEAT(JobManager.workers.length, i)
    {
        Worker* worker = JobManager.workers.data + i;
        Ensure(thrd_join(worker->thread, NULL) == thrd_success);
    }

    REPEAT(JobManager.jobCompletionConditions.length, i)
        cnd_destroy(JobManager.jobCompletionConditions.data + i);

    REPEAT(JobManager.jobs._.length, i)
        if(JobManager.jobs._.data[i].inUse)
            RemoveJob(i);

    mtx_destroy(&JobManager.mutex);
    cnd_destroy(&JobManager.updateCondition);
    DestroyArray(&JobManager.workers);
    DestroyFixedArray(&JobManager.jobs);
    DestroyArray(&JobManager.jobCompletionConditions);
    DestroyArray(&JobManager.jobQueue);

    memset(&JobManager, 0, sizeof(JobManager));
}

void LockJobManager()
{
    Ensure(mtx_lock(&JobManager.mutex) == thrd_success);
}

void UnlockJobManager()
{
    Ensure(mtx_unlock(&JobManager.mutex) == thrd_success);
}

void WaitForJobs( const JobId* jobIds, int jobIdCount )
{
    REPEAT(jobIdCount, i)
    {
        const JobId id = jobIds[i];
        while(GetJobStatus(id) != COMPLETED_JOB)
        {
            cnd_t* completionCondition =
                GetArrayElement(&JobManager.jobCompletionConditions, id);
            Ensure(cnd_wait(completionCondition, &JobManager.mutex) == thrd_success);
        }
    }
}

JobId CreateJob( JobConfig config )
{
    FixedArrayAllocation<Job> allocation = AllocateInFixedArray(&JobManager.jobs);
    Job* job = allocation.element;
    job->status = QUEUED_JOB;
    job->config = config;
    IncreaseCounter(JobCounter, 1);

    const JobId id = (JobId)allocation.pos;

    // Ensure that a condition variable is available:
    for(int i = JobManager.jobCompletionConditions.length; i <= id; i++)
    {
        cnd_t* completionCondition =
            AllocateAtEndOfArray(&JobManager.jobCompletionConditions, 1);
        cnd_init(completionCondition);
    }

    // Insert into job queue:
    InsertInArray(&JobManager.jobQueue, 0, 1, &id);

    cnd_signal(&JobManager.updateCondition);
    return id;
}

void RemoveJob( JobId jobId )
{
    const Job* job = GetFixedArrayElement(&JobManager.jobs, jobId);
    Ensure(job->status == COMPLETED_JOB);
    if(job->config.destructor)
        job->config.destructor(job->config.data);
    RemoveFromFixedArray(&JobManager.jobs, jobId);
    DecreaseCounter(JobCounter, 1);
}

JobStatus GetJobStatus( JobId jobId )
{
    return GetFixedArrayElement(&JobManager.jobs, jobId)->status;
}

void* GetJobData( JobId jobId )
{
    return GetFixedArrayElement(&JobManager.jobs, jobId)->config.data;
}

static Job* GetJob( JobId jobId )
{
    return GetFixedArrayElement(&JobManager.jobs, jobId);
}


// --- Worker specific ---

static JobId TryToGetQueuedJob()
{
    if(JobManager.jobQueue.length == 0)
        return INVALID_JOB_ID;

    const int queueIndex = JobManager.jobQueue.length - 1;
    const JobId id = JobManager.jobQueue.data[queueIndex];

    Job* job = GetFixedArrayElement(&JobManager.jobs, id);
    Ensure(job->status == QUEUED_JOB);

    RemoveFromArray(&JobManager.jobQueue, queueIndex, 1);

    job->status = ACTIVE_JOB;
    return id;
}

struct UpdateResult
{
    bool shallStop;
    JobId jobId;
};

static bool GetUpdate( UpdateResult* update )
{
    if(JobManager.isStopping)
    {
        update->shallStop = true;
        return true;
    }

    const JobId jobId = TryToGetQueuedJob();
    if(jobId != INVALID_JOB_ID)
    {
        update->jobId = jobId;
        return true;
    }

    return false;
}

static int WorkerThreadFn( void* arg_ )
{
    WorkerArgument* arg = (WorkerArgument*)arg_;
    NotifyProfilerAboutThreadCreation(Format("Worker %d", arg->workerId));
    DELETE(arg);

    for(;;)
    {
        LockJobManager(); // Can be released at different locations!

        UpdateResult update;
        memset(&update, 0, sizeof(update));
        while(!GetUpdate(&update))
            Ensure(cnd_wait(&JobManager.updateCondition, &JobManager.mutex) == thrd_success);

        JobConfig jobConfig; // May be uninitialized! See below ...
        if(!update.shallStop)
            jobConfig = GetJob(update.jobId)->config;

        UnlockJobManager();

        if(update.shallStop)
        {
            break;
        }
        else
        {
            jobConfig.processor(jobConfig.data);

            LockJobManager();

            Job* job = GetJob(update.jobId);
            Ensure(job->status == ACTIVE_JOB);
            job->status = COMPLETED_JOB;

            cnd_t* completionCondition =
                GetArrayElement(&JobManager.jobCompletionConditions, update.jobId);
            cnd_broadcast(completionCondition);

            UnlockJobManager();
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
