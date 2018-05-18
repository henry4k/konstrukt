#ifndef __KONSTRUKT_JOB_MANAGER__
#define __KONSTRUKT_JOB_MANAGER__

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
 * The creating thread has initially a lock on the job manager.
 */
void InitJobManager( JobManagerConfig config );

void DestroyJobManager();

/**
 * Must be run before accessing the job manager.
 */
void LockJobManager();
void UnlockJobManager();

/**
 * Block till all given jobs are completed.
 *
 * The job manager gets unlocked while waiting and is locked again when
 * control is returned to the calling thread.
 */
void WaitForJobs( const JobId* jobIds, int jobIdCount );


// --- Job ---

struct JobConfig
{
    const char* name; // Useful when debugging the engine.
    void (*processor)( void* data );
    void (*destructor)( void* data );
    void* data;
};

JobId CreateJob( JobConfig config );

void RemoveJob( JobId jobId );

JobStatus GetJobStatus( JobId jobId );

/**
 * May only be called on completed jobs.
 */
void* GetJobData( JobId jobId );


// --- Job function ---

// TODO: Service function, which can signal the job, that it needs to be
// suspended or stop.
// TODO: Use longjmp to suspend jobs?  Suspended jobs may need to be locked to
// their original (working) thread.

// For debugging purposes.
void Sleep( double seconds );


#endif
