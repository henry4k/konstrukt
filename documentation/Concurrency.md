Concurrency
===========

Abstract
--------

This describes how the engine distributes work to multiple CPU cores.

The engine uses a job system for this.  Work is stored in a job queue and
an arbitrary amount of worker threads processes them one after another.


Simulation
----------

A simulation tick consists of a sequential and a parallel phase.

First the sequential phase is used for inter-module communication and
synchronization.  At the end the modules can enqueue jobs, which will then do
the heavy lifting concurrently - e. g. physics and graphics computations.

It looks roughly like this:

```c
while(!WindowShouldClose())
{
    // ... inter-module communication ...

    BeginLuaUpdate(JobManagerInstance);
    BeginAudioUpdate(JobManagerInstance);

    CompleteLuaUpdate(JobManagerInstance);
    CompleteAudioUpdate(JobManagerInstance);
}
```

In the `Begin*Update` function the module prepares and enqueues its jobs.  As
other modules may already have enqueued their jobs, this function must only
read from other modules to change its internal state - the public state may
not be changed at this point anymore.  E.g. the audio module will update
positions of sounds that are attached to solids.  Then it enqueues a job which
will take care of sound processing and output.

In the `Complete*Update` function the module blocks the main thread, until its
jobs are done.  Additionally some modules may need to unpack job results.
As other modules may still be waiting for their jobs, this function must not
access any other modules.

![](Concurrency.png)
