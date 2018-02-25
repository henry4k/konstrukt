#ifndef __KONSTRUKT_SIMULATION_GROUP__
#define __KONSTRUKT_SIMULATION_GROUP__

struct JobManager;


struct SimulationGroup;

typedef void (*BeginUpdateFn)( void* context, double duration );
typedef void (*CompleteUpdateFn)( void* context );
typedef unsigned int SimulationId;


SimulationGroup* CreateSimulationGroup();
void DestroySimulationGroup( SimulationGroup* group );

SimulationId AddSimulationToGroup( SimulationGroup* group,
                                   void* context,
                                   BeginUpdateFn beginUpdate,
                                   CompleteUpdateFn completeUpdate );
void RemoveSimulationFromGroup( SimulationGroup* group, SimulationId id );

void SetSimulationSpeed( SimulationGroup* group, double factor );

void BeginSimulationGroupUpdate( SimulationGroup* group, double duration );
void CompleteSimulationGroupUpdate( SimulationGroup* group );


#endif
