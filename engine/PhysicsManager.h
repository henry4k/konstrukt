#ifndef __APOAPSIS_PHYSICS_MANAGER__
#define __APOAPSIS_PHYSICS_MANAGER__

#include "Math.h"


/**
 * Defines the shape of a #Solid.
 *
 * There are different types of collision shapes, each of them has its own
 * constructor function.
 */
struct CollisionShape;

/**
 * A body that is simulated by the physics engine.
 *
 * If a solid has no mass (i.e. it equals zero), it is concidered to be static.
 * So it isn't affected by collisions with other solids or gravity. Also each
 * solid needs a #CollisionShape, but try to reuse collision shapes to save
 * memory.
 */
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

/**
 * Changes a solids restitution factor, which defines its 'bouncyness'.
 */
void SetSolidRestitution( const Solid* solid, float restitution );

void SetSolidFriction( const Solid* solid, float friction );

glm::vec3 GetSolidPosition( const Solid* solid );

glm::quat GetSolidRotation( const Solid* solid );

/**
 * Writes a solids transformation in the matrix `target`.
 *
 * This can be used as model transformation in the render system to draw
 * graphics objects at the correct position.
 */
void GetSolidTransformation( const Solid* solid, glm::mat4* target );

/**
 * Velocity at which the solid moves through space.
 */
glm::vec3 GetSolidLinearVelocity( const Solid* solid );

/**
 * Velocity at which the solid rotates around itself.
 */
glm::vec3 GetSolidAngularVelocity( const Solid* solid );

#endif
