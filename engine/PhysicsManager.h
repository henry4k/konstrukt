#ifndef __APOAPSIS_PHYSICS_MANAGER__
#define __APOAPSIS_PHYSICS_MANAGER__

#include "Math.h"


struct Solid;


bool InitPhysicsManager();
void DestroyPhysicsManager();
void UpdatePhysicsManager( double timeDelta );

Solid* CreateSolid();
void FreeSolid( Solid* solid );

#endif
