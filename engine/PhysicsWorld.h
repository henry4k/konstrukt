#ifndef __KONSTRUKT_PHYSICS_WORLD__
#define __KONSTRUKT_PHYSICS_WORLD__

#include <float.h> // FLOAT_MAX
#include "Math.h"


struct JobManager;


/**
 * Simulates body collision and movement.
 */
struct PhysicsWorld;

/**
 * Defines the shape of a solid.
 *
 * There are different types of collision shapes, each of them has its own
 * constructor function.
 */
struct CollisionShape;

/**
 * A body that is simulated by the physics engine.
 *
 * Each solid needs a #CollisionShape, but try to reuse collision shapes to
 * save memory.
 */
typedef unsigned int SolidId;

/**
 * Changes the velocity of a #SolidId over time.
 *
 * Here a force is defined by its vector and position. The vector defines the
 * Forces that are at the center will only change the solids linear velocity,
 * while forces that are applied off-center will also change the angular
 * velocity. They are applied in each simulation step until you 'destroy' them.
 */
typedef unsigned int ForceId;


struct SolidProperties
{
    /**
     * If a solid has no mass (i.e. it equals zero), it is concidered to be
     * static.  So it isn't affected by collisions with other solids or
     * gravity.
     */
    float mass;

    /**
     * The restitution factor defines the solids 'bouncyness'.
     */
    float restitution;

    float friction;

    /**
     * Only collisions with an impulse greater than `thresholdImpulse` will trigger
     * the #CollisionCallback.
     */
    float collisionThreshold;

    /**
     * Only solids which have gravity enabled will be affected by it.
     */
    bool affectedByGravity;
};

struct SolidMotionState
{
    Vec3 position;
    Quat rotation;

    /**
     * Velocity at which the solid moves through space.
     */
    Vec3 linearVelocity;

    /**
     * Velocity at which the solid rotates around itself.
     */
    Vec3 angularVelocity;
};

struct Collision
{
    SolidId a;
    SolidId b;
    Vec3 pointOnA;
    Vec3 pointOnB;
    Vec3 normalOnB;
    float impulse;
};

static const float INFINITE_COLLISION_THRESHOLD = FLT_MAX; // Safer than INFINITY

typedef void (*CollisionCallback)( PhysicsWorld* world, const Collision* collision );


PhysicsWorld* CreatePhysicsWorld();
void DestroyPhysicsWorld( PhysicsWorld* world );
void BeginPhysicsWorldUpdate( PhysicsWorld* world, JobManager* jobManager, double timeDelta );
void CompletePhysicsWorldUpdate( PhysicsWorld* world, JobManager* jobManager );
void SetCollisionCallback( CollisionCallback callback );
void SetGravity( PhysicsWorld* world, Vec3 force );


CollisionShape* CreateEmptyCollisionShape();
CollisionShape* CreateBoxCollisionShape( Vec3 halfWidth );
CollisionShape* CreateSphereCollisionShape( float radius );
CollisionShape* CreateCapsuleCollisionShape( float radius, float height );
CollisionShape* CreateCompoundCollisionShape( int shapeCount,
                                              CollisionShape** shapes,
                                              Vec3* positions );

void ReferenceCollisionShape( CollisionShape* shape );
void ReleaseCollisionShape( CollisionShape* shape );


SolidId CreateSolid( PhysicsWorld* world,
                     const SolidProperties* properties,
                     const SolidMotionState* motionState,
                     CollisionShape* shape );

void ReferenceSolid( PhysicsWorld* world, SolidId solid );
void ReleaseSolid( PhysicsWorld* world, SolidId solid );

void SetSolidProperties( PhysicsWorld* world,
                         SolidId solid,
                         const SolidProperties* properties );

/**
 * Pointer is valid till the next call to #CompletePhysicsWorldUpdate.
 */
const SolidMotionState* GetSolidMotionState( PhysicsWorld* world, SolidId solid );

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
void ApplySolidImpulse( PhysicsWorld* world,
                        SolidId solid,
                        Vec3 impulse,
                        Vec3 relativePosition,
                        bool useLocalCoordinates );

/**
 * Initially all properties are zero, so that the force has no effect.
 *
 * @return
 * The handle or `NULL` if the force couldn't be created.
 */
ForceId CreateForce( PhysicsWorld* world, SolidId solid );

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
void SetForce( PhysicsWorld* world,
               ForceId force,
               Vec3 value,
               Vec3 relativePosition,
               bool useLocalCoordinates );

void DestroyForce( PhysicsWorld* world, ForceId force );

#endif
