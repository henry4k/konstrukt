#ifndef __APOAPSIS_PHYSICS_MANAGER__
#define __APOAPSIS_PHYSICS_MANAGER__

#include <float.h> // FLOAT_MAX
#include "Math.h"


/**
 * Defines the shape of a #Solid.
 *
 * There are different types of collision shapes, each of them has its own
 * constructor function.
 */
struct CollisionShape;

/**
 * Changes the velocity of a solid over time.
 *
 * Here a force is defined by its vector and position. The vector defines the
 * Forces that are at the center will only change the solids linear velocity,
 * while forces that are applied off-center will also change the angular
 * velocity. They are applied in each simulation step until you 'destroy' them.
 */
struct Force;

/**
 * A body that is simulated by the physics engine.
 *
 * If a solid has no mass (i.e. it equals zero), it is concidered to be static.
 * So it isn't affected by collisions with other solids or gravity. Also each
 * solid needs a #CollisionShape, but try to reuse collision shapes to save
 * memory.
 */
struct Solid;


struct Collision
{
    Solid* a;
    Solid* b;
    glm::vec3 pointOnA;
    glm::vec3 pointOnB;
    glm::vec3 normalOnB;
    float impulse;
};

static const float INFINITE_COLLISION_THRESHOLD = FLT_MAX; // Safer than INFINITY

typedef void (*CollisionCallback)( const Collision* collision );


bool InitPhysicsManager();
void DestroyPhysicsManager();
void UpdatePhysicsManager( double timeDelta );
void SetCollisionCallback( CollisionCallback callback );
void SetGravity( glm::vec3 force );


CollisionShape* CreateEmptyCollisionShape();
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

/**
 * Only collisions with an impulse greater than `thresholdImpulse` will trigger
 * the #CollisionCallback. The default threshold is #INFINITE_COLLISION_THRESHOLD.
 */
void SetSolidCollisionThreshold( Solid* solid, float threshold );

glm::vec3 GetSolidPosition( const Solid* solid );

glm::quat GetSolidRotation( const Solid* solid );

/**
 * Writes a solids transformation in the matrix `target`.
 *
 * This can be used as model transformation in the render system to draw
 * graphics objects at the correct position.
 *
 * @param copyFlags
 * Any flag combination, that is valid for #GetTransformation.
 */
void GetSolidTransformation( const Solid* solid, int copyFlags, glm::mat4* target );

/**
 * Velocity at which the solid moves through space.
 */
glm::vec3 GetSolidLinearVelocity( const Solid* solid );

/**
 * Velocity at which the solid rotates around itself.
 */
glm::vec3 GetSolidAngularVelocity( const Solid* solid );

/**
 * Instantly applies an impulse.
 *
 * In contrast to forces, impulses are independent of the simulation rate.
 *
 * @param impulse
 * Describes the magnitude and direction.
 *
 * @param relativePosition
 * Point where the impulse is applied to the solid.
 *
 * @param useLocalCoordinates
 * If set direction and position will be relative to the solids orientation.
 */
void ApplySolidImpulse( const Solid* solid,
                        glm::vec3 impulse,
                        glm::vec3 relativePosition,
                        bool useLocalCoordinates );

/**
 * Initially all properties are zero, so that the force has no effect.
 *
 * @return
 * The handle or `NULL` if the force couldn't be created.
 */
Force* CreateForce( Solid* solid );

/**
 * Changes the properties of the force.
 *
 * @param value
 * Describes the magnitude and direction that is applied in one second.
 *
 * @param relativePosition
 * Point where the force is applied to the solid.
 *
 * @param useLocalCoordinates
 * If set direction and position will be relative to the solids orientation.
 */
void SetForce( Force* force,
               glm::vec3 value,
               glm::vec3 relativePosition,
               bool useLocalCoordinates );

void DestroyForce( Force* force );

#endif
