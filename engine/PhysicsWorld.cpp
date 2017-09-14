#include <string.h> // memset, memcpy

#include "Warnings.h"
BEGIN_EXTERNAL_CODE
#include <BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.h>
#include <BulletCollision/CollisionDispatch/btCollisionDispatcher.h>
#include <BulletCollision/BroadphaseCollision/btDbvtBroadphase.h>
#include <BulletCollision/CollisionShapes/btCollisionShape.h>
#include <BulletCollision/CollisionShapes/btEmptyShape.h>
#include <BulletCollision/CollisionShapes/btBoxShape.h>
#include <BulletCollision/CollisionShapes/btSphereShape.h>
#include <BulletCollision/CollisionShapes/btCapsuleShape.h>
#include <BulletCollision/CollisionShapes/btCompoundShape.h>
#include <BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.h>
#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#include <BulletDynamics/Dynamics/btRigidBody.h>
#include <LinearMath/btDefaultMotionState.h>
END_EXTERNAL_CODE

#include "Common.h"
#include "ObjectSystem.h"
#include "Profiler.h"
#include "Constants.h" // SIMULATION_FREQUENCY, MAX_FRAME_FREQUENCY
#include "Math.h"
#include "Reference.h"
#include "JobManager.h"
#include "PhysicsWorld.h"


enum CollisionShapeType
{
    EMPTY_SHAPE,
    BOX_SHAPE,
    SPHERE_SHAPE,
    CAPSULE_SHAPE,
    COMPOUND_SHAPE
};

struct CollisionShape
{
    ReferenceCounter refCounter;
    CollisionShapeType type;
    btCollisionShape* bulletInstance;
};

struct Force
{
    SolidId solidId;
    Vec3 value;
    Vec3 relativePosition;
    bool useLocalCoordinates;
};

struct Solid
{
    ReferenceCounter refCounter;
    btRigidBody* rigidBody;
    CollisionShape* collisionShape;
    float collisionThreshold;
    SolidMotionState motionState;
};

struct PhysicsWorld
{
    btCollisionConfiguration* collisionConfiguration;
    btCollisionDispatcher* collisionDispatcher;
    btBroadphaseInterface* broadphaseInterface;
    btConstraintSolver* constraintSolver;
    btDiscreteDynamicsWorld* dynamicsWorld;

    ObjectSystem<Solid> solids;
    ObjectSystem<Force> forces;

    JobId updateJob;
    double updateDuration;
};

static CollisionCallback CurrentCollisionCallback = NULL;


static void FreeSolid( Solid* solid );
static void DestroyAllFocesOfSolid( PhysicsWorld* world, SolidId solidId );
static void WorldTickCallback( btDynamicsWorld* world, btScalar duration );
static void HandleCollisions( PhysicsWorld* world );
static void ApplyForces( PhysicsWorld* world, float duration );
static void UpdateSolidMotionStates( PhysicsWorld* world );

static inline btVector3 ToBulletVec( const Vec3& v )
{
    return btVector3(v._[0], v._[1], v._[2]);
}

static inline Vec3 FromBulletVec( const btVector3& v )
{
    const Vec3 r = {{v.getX(), v.getY(), v.getZ()}};
    return r;
}

static inline btQuaternion ToBulletQuat( const Quat& q )
{
    return btQuaternion(q._[0],
                        q._[1],
                        q._[2],
                        q._[3]);
}

static inline Quat FromBulletQuat( const btQuaternion& q )
{
    const Quat r = {{q.getX(),
                     q.getY(),
                     q.getZ(),
                     q.getW()}};
    return r;
}


// --- PhysicsWorld ---

PhysicsWorld* CreatePhysicsWorld()
{
    const int version = btGetVersion();
    const int major = version/100;
    const int minor = (version-major*100)/10;
    const int patch = version-(major*100 + minor*10);
    LogInfo("Using Bullet %d.%d.%d", major, minor, patch);

    // TODO:
    // btDiscreteDynamicsWorldMt(btDispatcher* dispatcher,btBroadphaseInterface* pairCache,btConstraintSolver* constraintSolver,btCollisionConfiguration* collisionConfiguration);
    // class btSimulationIslandManagerMt : public btSimulationIslandManager
    // typedef void( *IslandDispatchFunc ) ( btAlignedObjectArray<Island*>* islands, IslandCallback* callback );

    PhysicsWorld* world = NEW(PhysicsWorld);
    world->collisionConfiguration = new btDefaultCollisionConfiguration();
    world->collisionDispatcher = new btCollisionDispatcher(world->collisionConfiguration);
    world->broadphaseInterface = new btDbvtBroadphase();
    world->constraintSolver = new btSequentialImpulseConstraintSolver();
    world->dynamicsWorld = new btDiscreteDynamicsWorld(world->collisionDispatcher,
                                                       world->broadphaseInterface,
                                                       world->constraintSolver,
                                                       world->collisionConfiguration);

    world->dynamicsWorld->setWorldUserInfo(world);

    InitObjectSystem(&world->solids);
    InitObjectSystem(&world->forces);

    world->dynamicsWorld->setInternalTickCallback(WorldTickCallback);

    return world;
}

void DestroyPhysicsWorld( PhysicsWorld* world )
{
    DestroyObjectSystem(&world->solids);
    DestroyObjectSystem(&world->forces);

    delete world->dynamicsWorld;
    delete world->constraintSolver;
    delete world->collisionDispatcher;
    delete world->collisionConfiguration;
    delete world->broadphaseInterface;

    DELETE(world);
}

struct PhysicsWorldUpdateJob
{
    PhysicsWorld* world;
    double duration;
};

static void UpdatePhysicsWorld( void* _data )
{
    ProfileFunction();

    PhysicsWorld* world = (PhysicsWorld*)_data;
    const double duration = world->updateDuration;

    static const int maxSteps = MAX_FRAME_FREQUENCY / SIMULATION_FREQUENCY;
    static const double stepDuration = 1.0 / (double)SIMULATION_FREQUENCY;
    world->dynamicsWorld->stepSimulation(duration, maxSteps, stepDuration);
    ApplyForces(world, duration);
}

void BeginPhysicsWorldUpdate( PhysicsWorld* world, JobManager* jobManager, double duration )
{
    world->updateJob = CreateJob(jobManager, {"UpdatePhysicsWorld", UpdatePhysicsWorld, world});
}

void CompletePhysicsWorldUpdate( PhysicsWorld* world, JobManager* jobManager )
{
    WaitForJobs(jobManager, &world->updateJob, 1);
    UpdateSolidMotionStates(world);
}

void SetGravity( PhysicsWorld* world, Vec3 force )
{
    world->dynamicsWorld->setGravity(ToBulletVec(force));
}

static void WorldTickCallback( btDynamicsWorld* dynamicsWorld, btScalar duration )
{
    PhysicsWorld* world = (PhysicsWorld*)dynamicsWorld->getWorldUserInfo();
    HandleCollisions(world);
}


// --- CollisionShape ---

static CollisionShape* CreateCollisionShape( CollisionShapeType type, btCollisionShape* bulletInstance )
{
    CollisionShape* shape = NEW(CollisionShape);
    shape->type = type;
    InitReferenceCounter(&shape->refCounter);
    shape->bulletInstance = bulletInstance;
    shape->bulletInstance->setUserPointer(shape);
    return shape;
}

CollisionShape* CreateEmptyCollisionShape()
{
    return CreateCollisionShape(EMPTY_SHAPE,
                                new btEmptyShape());
}

CollisionShape* CreateBoxCollisionShape( Vec3 halfWidth )
{
    return CreateCollisionShape(BOX_SHAPE,
                                new btBoxShape(ToBulletVec(halfWidth)));
}

CollisionShape* CreateSphereCollisionShape( float radius )
{
    return CreateCollisionShape(SPHERE_SHAPE,
                                new btSphereShape(radius));
}

CollisionShape* CreateCapsuleCollisionShape( float radius, float height )
{
    return CreateCollisionShape(CAPSULE_SHAPE,
                                new btCapsuleShape(radius, height));
}

CollisionShape* CreateCompoundCollisionShape( int shapeCount, CollisionShape** shapes, const Vec3* positions )
{
    btCompoundShape* bulletInstance = new btCompoundShape(false);
    REPEAT(shapeCount, i)
    {
        CollisionShape* shape = shapes[i];
        ReferenceCollisionShape(shape);
        const btTransform transform(btQuaternion(),
                                    btVector3(ToBulletVec(positions[i])));
        bulletInstance->addChildShape(transform, shape->bulletInstance);
    }
    return CreateCollisionShape(COMPOUND_SHAPE, bulletInstance);
}

static void FreeCollisionShape( CollisionShape* shape )
{
    if(shape->type == COMPOUND_SHAPE)
    {
        const btCompoundShape* compound = (btCompoundShape*)shape->bulletInstance;
        const int childCount = compound->getNumChildShapes();
        REPEAT(childCount, i)
        {
            const btCollisionShape* child = compound->getChildShape(i);
            CollisionShape* childShape = (CollisionShape*)child->getUserPointer();
            ReleaseCollisionShape(childShape);
        }
    }
    delete shape->bulletInstance;
    delete shape;
}

void ReferenceCollisionShape( CollisionShape* shape )
{
    Reference(&shape->refCounter);
}

void ReleaseCollisionShape( CollisionShape* shape )
{
    Release(&shape->refCounter);
    if(!HasReferences(&shape->refCounter))
        FreeCollisionShape(shape);
}


// --- Solid ---

static btVector3 CalcLocalInertia( const CollisionShape* shape, float mass )
{
    if(shape->type == EMPTY_SHAPE)
    {
        const float v = 0.4f * mass * 0.5f; // simulate a sphere with radius 0.5
        return btVector3(v,v,v);
    }
    else
    {
        btVector3 localInertia;
        shape->bulletInstance->calculateLocalInertia(mass, localInertia);
        return localInertia;
    }
}

static void EnableGravityForBulletRigidBody( btRigidBody* rigidBody, bool enable )
{
    const int flags = rigidBody->getFlags();
    if(enable)
        rigidBody->setFlags(flags & ~BT_DISABLE_WORLD_GRAVITY);
    else
        rigidBody->setFlags(flags | BT_DISABLE_WORLD_GRAVITY);
}

static btRigidBody* CreateBulletRigidBody( const SolidProperties* properties,
                                           const SolidMotionState* motionState,
                                           const CollisionShape* shape )
{
    btMotionState* bulletMotionState =
        new btDefaultMotionState(btTransform(ToBulletQuat(motionState->rotation),
                                             ToBulletVec(motionState->position)));

    const btVector3 localInertia = CalcLocalInertia(shape, properties->mass);

    btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(properties->mass,
                                                         bulletMotionState,
                                                         shape->bulletInstance,
                                                         localInertia);

    btRigidBody* rigidBody = new btRigidBody(rigidBodyCI);

    rigidBody->setLinearVelocity(ToBulletVec(motionState->linearVelocity));
    rigidBody->setAngularVelocity(ToBulletVec(motionState->angularVelocity));
    rigidBody->setRestitution(properties->restitution);
    rigidBody->setFriction(properties->friction);
    EnableGravityForBulletRigidBody(rigidBody, properties->affectedByGravity);

    return rigidBody;
}

static_assert(sizeof(SolidId) <= sizeof(int), "");
union SolidUserIndexHelper
{
    SolidId id;
    int index;
};

SolidId CreateSolid( PhysicsWorld* world,
                     const SolidProperties* properties,
                     const SolidMotionState* motionState,
                     CollisionShape* shape )
{
    SolidId id = AllocateObject(&world->solids);
    Solid* solid = GetObject(&world->solids, id);
    memset(solid, 0, sizeof(Solid));

    InitReferenceCounter(&solid->refCounter);

    solid->rigidBody = CreateBulletRigidBody(properties, motionState, shape);
    solid->collisionShape = shape;
    solid->collisionThreshold = properties->collisionThreshold;
    solid->motionState = *motionState;

    SolidUserIndexHelper helper;
    helper.id = id;
    solid->rigidBody->setUserIndex(helper.index);

    world->dynamicsWorld->addRigidBody(solid->rigidBody);

    return id;
}

static SolidId GetSolidIdFromCollisionObject( const btCollisionObject* co )
{
    SolidUserIndexHelper helper;
    helper.index = co->getUserIndex();
    return helper.id;
}

static void RemoveSolid( PhysicsWorld* world, SolidId solidId, Solid* solid )
{
    DestroyAllFocesOfSolid(world, solidId);

    world->dynamicsWorld->removeRigidBody(solid->rigidBody);
    ReleaseCollisionShape(solid->collisionShape);
    delete solid->rigidBody->getMotionState();
    delete solid->rigidBody;

    RemoveObject(&world->solids, solidId);
}

void ReferenceSolid( PhysicsWorld* world, SolidId solidId )
{
    Reference(&GetObject(&world->solids, solidId)->refCounter);
}

void ReleaseSolid( PhysicsWorld* world, SolidId solidId )
{
    Solid* solid = GetObject(&world->solids, solidId);
    Release(&solid->refCounter);
    if(!HasReferences(&solid->refCounter))
        RemoveSolid(world, solidId, solid);
}

void SetSolidProperties( PhysicsWorld* world,
                         SolidId solidId,
                         const SolidProperties* properties )
{
    Solid* solid = GetObject(&world->solids, solidId);

    // Mass:
    const float mass = properties->mass;
    solid->rigidBody->setMassProps(mass, CalcLocalInertia(solid->collisionShape, mass));

    // Gravity:
    EnableGravityForBulletRigidBody(solid->rigidBody,
                                    properties->affectedByGravity);

    solid->rigidBody->setRestitution(properties->restitution);
    solid->rigidBody->setFriction(properties->friction);
    solid->collisionThreshold = properties->collisionThreshold;
}

const SolidMotionState* GetSolidMotionState( PhysicsWorld* world, SolidId solidId )
{
    const Solid* solid = GetObject(&world->solids, solidId);
    return &solid->motionState;
}

static void UpdateSolidMotionState( Solid* solid )
{
    btRigidBody* rigidBody = solid->rigidBody;

    if(!solid->rigidBody->isActive())
        return;

    SolidMotionState* motionState = &solid->motionState;

    btTransform transform;
    rigidBody->getMotionState()->getWorldTransform(transform);

    motionState->position = FromBulletVec(transform.getOrigin());
    motionState->rotation = FromBulletQuat(transform.getRotation());
    motionState->linearVelocity  = FromBulletVec(rigidBody->getLinearVelocity());
    motionState->angularVelocity = FromBulletVec(rigidBody->getAngularVelocity());
}

static void UpdateSolidMotionStates( PhysicsWorld* world )
{
    REPEAT(GetObjectCount(&world->solids), i)
    {
        Solid* solid = GetObjectByIndex(&world->solids, i);
        UpdateSolidMotionState(solid);
    }
}

static Mat4 GetSolidRotationMatrix( const Solid* solid )
{
    btTransform transform;
    solid->rigidBody->getMotionState()->getWorldTransform(transform);

    return Mat4FromQuat(FromBulletQuat(transform.getRotation()));
}

void ApplySolidImpulse( PhysicsWorld* world,
                        SolidId solidId,
                        Vec3 impulse,
                        Vec3 relativePosition,
                        bool useLocalCoordinates )
{
    Solid* solid = GetObject(&world->solids, solidId);
    const bool isCentral = ArraysAreEqual(relativePosition._,
                                          Vec3Zero._,
                                          3);
    if(useLocalCoordinates)
    {
        const Mat4 rotation = GetSolidRotationMatrix(solid);
        impulse          = MulMat4ByVec3(rotation, impulse);
        relativePosition = MulMat4ByVec3(rotation, relativePosition);
    }

    if(isCentral)
        solid->rigidBody->applyCentralImpulse(ToBulletVec(impulse));
    else
        solid->rigidBody->applyImpulse(ToBulletVec(impulse),
                                       ToBulletVec(relativePosition));
}


// --- Force ---

static void DestroyAllFocesOfSolid( PhysicsWorld* world, SolidId solidId )
{
    REPEAT(GetObjectCount(&world->forces), i)
    {
        Force* force = GetObjectByIndex(&world->forces, i);
        if(force->solidId == solidId)
        {
            const ForceId forceId = GetObjectIdByIndex(&world->forces, i);
            FatalError("Solid %u still had force %u on destruction.",
                       solidId, forceId);
            DestroyForce(world, forceId);
        }
    }
}

ForceId CreateForce( PhysicsWorld* world, SolidId solidId )
{
    Ensure(HasObject(&world->solids, solidId));

    ForceId id = AllocateObject(&world->forces);
    Force* force = GetObject(&world->forces, id);
    memset(force, 0, sizeof(Force));

    force->solidId = solidId;

    return id;
}

void SetForce( PhysicsWorld* world,
               ForceId forceId,
               Vec3 value,
               Vec3 relativePosition,
               bool useLocalCoordinates )
{
    Force* force = GetObject(&world->forces, forceId);
    force->value = value;
    force->relativePosition = relativePosition;
    force->useLocalCoordinates = useLocalCoordinates;
}

void DestroyForce( PhysicsWorld* world, ForceId forceId )
{
    RemoveObject(&world->forces, forceId);
}

static void ApplyForces( PhysicsWorld* world, float duration )
{
    REPEAT(GetObjectCount(&world->forces), i)
    {
        const Force* force = GetObjectByIndex(&world->forces, i);
        const Solid* solid = GetObjectByIndex(&world->solids, force->solidId);

        Vec3 value;
        REPEAT(3,j) { value._[j] = force->value._[j] * duration; }
        Vec3 relativePosition = force->relativePosition;

        const bool isCentral = ArraysAreEqual(relativePosition._,
                                              Vec3Zero._,
                                              3);

        if(force->useLocalCoordinates)
        {
            const Mat4 rotation = GetSolidRotationMatrix(solid);
            value            = MulMat4ByVec3(rotation, value);
            relativePosition = MulMat4ByVec3(rotation, relativePosition);
        }

        if(isCentral)
            solid->rigidBody->applyCentralForce(ToBulletVec(value));
        else
            solid->rigidBody->applyForce(ToBulletVec(value),
                                         ToBulletVec(relativePosition));
    }
}


// --- Collisions ---

void SetCollisionCallback( CollisionCallback callback )
{
    CurrentCollisionCallback = callback;
}

static inline bool PropagateCollision( const btManifoldPoint& point,
                                       const Solid* a, const Solid* b )
{
    const float impulse = point.m_appliedImpulse;
    const float thresholdA = a->collisionThreshold;
    const float thresholdB = b->collisionThreshold;
    return point.getDistance() < 0 && // Penetration
           (impulse >= thresholdA || impulse >= thresholdB); // At least one threshold is triggered
}

static void HandleCollisions( PhysicsWorld* world )
{
    if(!CurrentCollisionCallback)
        return;

    btDispatcher* dispatcher = world->dynamicsWorld->getDispatcher();
    REPEAT(dispatcher->getNumManifolds(), i)
    {
        const btPersistentManifold* manifold = dispatcher->getManifoldByIndexInternal(i);

        const btCollisionObject* coA = (const btCollisionObject*)manifold->getBody0();
        const btCollisionObject* coB = (const btCollisionObject*)manifold->getBody1();
        SolidId solidAId = GetSolidIdFromCollisionObject(coA);
        SolidId solidBId = GetSolidIdFromCollisionObject(coB);
        Solid* solidA = GetObject(&world->solids, solidAId);
        Solid* solidB = GetObject(&world->solids, solidBId);

        assert(solidA && solidB);

        REPEAT(manifold->getNumContacts(), j)
        {
            const btManifoldPoint& point = manifold->getContactPoint(j);
            if(PropagateCollision(point, solidA, solidB))
            {
                const btVector3& pointOnA  = point.getPositionWorldOnA();
                const btVector3& pointOnB  = point.getPositionWorldOnB();
                const btVector3& normalOnB = point.m_normalWorldOnB;

                Collision collision;
                collision.a = solidAId;
                collision.b = solidBId;
                collision.pointOnA  = FromBulletVec(pointOnA);
                collision.pointOnB  = FromBulletVec(pointOnB);
                collision.normalOnB = FromBulletVec(normalOnB);
                collision.impulse = point.m_appliedImpulse;

                CurrentCollisionCallback(world, &collision);
            }
        }
    }
}
