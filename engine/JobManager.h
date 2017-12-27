#ifndef __KONSTRUKT_JOB_MANAGER__
#define __KONSTRUKT_JOB_MANAGER__

struct JobManager;
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
     */
    int workerThreads;
};

/**
 * Creating thread has initially a lock on the job manager.
 */
JobManager* CreateJobManager( JobManagerConfig config );

void DestroyJobManager( JobManager* manager );

/**
 * Must be run before accessing the job manager.
 */
void LockJobManager( JobManager* manager );
void UnlockJobManager( JobManager* manager );

/**
 * Block till all given jobs are completed.
 *
 * The job manager gets unlocked while waiting and is locked again when
 * control is returned to the calling thread.
 */
void WaitForJobs( JobManager* manager, const JobId* jobIds, int jobIdCount );


// --- Job ---

struct JobConfig
{
    const char* name; // Useful when debugging the engine.
    void (*function)( void* data );
    void (*destructor)( void* data );
    void* data;
};

JobId CreateJob( JobManager* manager, JobConfig config );

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

// For debugging purposes.
void Sleep( double seconds );


#endif
