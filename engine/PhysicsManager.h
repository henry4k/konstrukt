#ifndef __APOAPSIS_PHYSICS_MANAGER__
#define __APOAPSIS_PHYSICS_MANAGER__

#include "Math.h"


struct CollisionShape;
struct Solid;


bool InitPhysicsManager();
void DestroyPhysicsManager();
void UpdatePhysicsManager( double timeDelta );


CollisionShape* CreateBoxCollisionShape( glm::vec3 halfWidth );
CollisionShape* CreateSphereCollisionShape( float radius );

void ReferenceCollisionShape( CollisionShape* shape );
void ReleaseCollisionShape( CollisionShape* shape );


Solid* CreateSolid( CollisionShape* shape );

void ReferenceSolid( Solid* solid );
void ReleaseSolid( Solid* solid );

glm::vec3 GetSolidPosition( const Solid* solid );
glm::vec3 GetSolidRotation( const Solid* solid );
void GetSolidTransformation( const Solid* solid, glm::mat4* target );

#endif
