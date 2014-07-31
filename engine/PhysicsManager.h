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
CollisionShape* CreateCapsuleCollisionShape( float radius, float height );
CollisionShape* CreateCompoundCollisionShape( int shapeCount,
                                              CollisionShape** shapes,
                                              glm::vec3* positions );

void ReferenceCollisionShape( CollisionShape* shape );
void ReleaseCollisionShape( CollisionShape* shape );


Solid* CreateSolid( float mass,
                    glm::vec3 position,
                    glm::quat rotation,
                    CollisionShape* shape );

void ReferenceSolid( Solid* solid );
void ReleaseSolid( Solid* solid );

float GetSolidMass( const Solid* solid );
void SetSolidMass( const Solid* solid, float mass );
glm::vec3 GetSolidPosition( const Solid* solid );
glm::quat GetSolidRotation( const Solid* solid );
void GetSolidTransformation( const Solid* solid, glm::mat4* target );

#endif
