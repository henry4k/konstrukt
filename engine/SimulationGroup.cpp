#include <assert.h>

#include "Common.h"
#include "ObjectSystem.h"
#include "SimulationGroup.h"


struct Simulation
{
    void* context;
    BeginUpdateFn beginUpdate;
    CompleteUpdateFn completeUpdate;
};

struct SimulationGroup
{
    double timeFactor;
    double totalTime;
    bool updateRunning; // true between begin and complete update calls
    JobManager* currentJobManager; // is set on BeginSimulationGroupUpdate
    ObjectSystem<Simulation> simulations;
};


SimulationGroup* CreateSimulationGroup()
{
    SimulationGroup* group = NEW(SimulationGroup);
    group->timeFactor = 1.0;
    group->totalTime = 0.0;
    group->updateRunning = false;
    group->currentJobManager = NULL;
    InitObjectSystem(&group->simulations);
    return group;
}

void DestroySimulationGroup( SimulationGroup* group )
{
    DestroyObjectSystem(&group->simulations);
    DELETE(group);
}

SimulationId AddSimulationToGroup( SimulationGroup* group,
                                   void* context,
                                   BeginUpdateFn beginUpdate,
                                   CompleteUpdateFn completeUpdate )
{
    const SimulationId id = AllocateObject(&group->simulations);
    Simulation* simulation = GetObject(&group->simulations, id);

    simulation->context = context;
    simulation->beginUpdate = beginUpdate;
    simulation->completeUpdate = completeUpdate;

    return id;
}

void RemoveSimulationFromGroup( SimulationGroup* group, SimulationId id )
{
    Ensure(!group->updateRunning);
    RemoveObject(&group->simulations, id);
}

void SetSimulationTimeFactor( SimulationGroup* group, double factor )
{
    Ensure(!group->updateRunning);
    group->timeFactor = factor;
}

void BeginSimulationGroupUpdate( SimulationGroup* group,
                                 JobManager* jobManager,
                                 double duration )
{
    Ensure(!group->updateRunning);
    assert(!group->currentJobManager);
    group->updateRunning = true;
    group->currentJobManager = jobManager;

    if(group->timeFactor != 0)
        return;

    duration *= group->timeFactor;
    group->totalTime += duration;

    REPEAT(GetObjectCount(&group->simulations), i)
    {
        const Simulation* simulation = GetObjectByIndex(&group->simulations, i);
        simulation->beginUpdate(simulation->context, jobManager, duration);
    }
}

void CompleteSimulationGroupUpdate( SimulationGroup* group )
{
    Ensure(group->updateRunning);
    assert(group->currentJobManager);

    JobManager* jobManager = group->currentJobManager;

    REPEAT(GetObjectCount(&group->simulations), i)
    {
        const Simulation* simulation = GetObjectByIndex(&group->simulations, i);
        simulation->completeUpdate(simulation->context, jobManager);
    }

    group->updateRunning = false;
    group->currentJobManager = NULL;
}
