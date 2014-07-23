#ifndef __APOAPSIS_PHYSICS_MANAGER__
#define __APOAPSIS_PHYSICS_MANAGER__

#include "Math.h"


struct Solid;


bool InitPhysicsManager();
void DestroyPhysicsManager();
void UpdatePhysicsManager( double timeDelta );

Solid* CreateSolid();

void ReferenceSolid( Solid* solid );
void ReleaseSolid( Solid* solid );

glm::vec3 GetSolidPosition( const Solid* solid );
glm::vec3 GetSolidRotation( const Solid* solid );
void GetSolidTransformation( const Solid* solid, glm::mat4* target );

#endif
