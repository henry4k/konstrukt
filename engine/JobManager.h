#ifndef __KONSTRUKT_JOB_MANAGER__
#define __KONSTRUKT_JOB_MANAGER__

struct JobManager;
typedef int JobTypeId;
typedef int JobId;

enum JobStatus
{
    QUEUED_JOB,
    ACTIVE_JOB,
    COMPLETED_JOB
};


struct JobManagerConfig
{
    /**
     * Number of worker threads, that are spawned.
     *
     * Possible values are:
     *
     * - a positive number
     * - zero (you need to call `RunJobs` manually)
     * - a negative number: number of CPU cores minus X
     *
     * So with a quadcore CPU and `workerThreads` set to `-1` you would end up
     * with 3 worker threads.  This way the remaining core can be used by the
     * main thread.
     */
    int workerThreads;
};

JobManager* CreateJobManager( JobManagerConfig config );

void DestroyJobManager( JobManager* manager );

/**
 * Use the current thread to work on jobs for a given duration.
 */
void RunJobs( double duration );


// --- JobType ---

struct JobTypeConfig
{
    const char* name; // Useful when debugging the engine.
    void (*function)( void* data );
};

JobTypeId CreateJobType( JobManager* manager, JobTypeConfig config );


// --- Job ---

JobId CreateJob( JobManager* manager, JobTypeId typeId, void* data );

void RemoveJob( JobManager* manager, JobId jobId );

JobStatus GetJobStatus( JobManager* manager, JobId jobId );

/**
 * May only be called on completed jobs.
 */
void* GetJobData( JobManager* manager, JobId jobId );


// --- Job function ---

// TODO: Service function, which can signal the job, that it needs to be
// suspended or stop.
// TODO: Use longjmp to suspend jobs?  Suspended jobs may need to be locked to
// their original (working) thread.


#endif
