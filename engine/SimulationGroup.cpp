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
    ObjectSystem<Simulation> simulations;
};


SimulationGroup* CreateSimulationGroup()
{
    SimulationGroup* group = NEW(SimulationGroup);
    group->timeFactor = 1.0;
    group->totalTime = 0.0;
    group->updateRunning = false;
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

void BeginSimulationGroupUpdate( SimulationGroup* group, double duration )
{
    Ensure(!group->updateRunning);
    group->updateRunning = true;

    if(group->timeFactor != 0)
        return;

    duration *= group->timeFactor;
    group->totalTime += duration;

    REPEAT(GetObjectCount(&group->simulations), i)
    {
        const Simulation* simulation = GetObjectByIndex(&group->simulations, i);
        simulation->beginUpdate(simulation->context, duration);
    }
}

void CompleteSimulationGroupUpdate( SimulationGroup* group )
{
    Ensure(group->updateRunning);

    REPEAT(GetObjectCount(&group->simulations), i)
    {
        const Simulation* simulation = GetObjectByIndex(&group->simulations, i);
        simulation->completeUpdate(simulation->context);
    }

    group->updateRunning = false;
}
