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
#include "Constants.h" // SIMULATION_FREQUENCY, MAX_FRAME_FREQUENCY
#include "Math.h"
#include "Reference.h"
#include "PhysicsManager.h"


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
    Solid* solid;
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
};


static btCollisionConfiguration* CollisionConfiguration = NULL;
static btCollisionDispatcher* CollisionDispatcher = NULL;
static btBroadphaseInterface* BroadphaseInterface = NULL;
static btConstraintSolver* ConstraintSolver = NULL;
static btDiscreteDynamicsWorld* World = NULL;

static const int MAX_FORCES = 8;
static Force Forces[MAX_FORCES];

static CollisionCallback CurrentCollisionCallback = NULL;


static void FreeSolid( Solid* solid );
static void DestroyAllFocesOfSolid( Solid* solid );
static void WorldTickCallback( btDynamicsWorld* world, btScalar timeDelta );
static void HandleCollisions();
static void ApplyForces( float timeDelta );

static inline btVector3 ToBulletVec( const Vec3& v )
{
    return btVector3(v._[0], v._[1], v._[2]);
}

static inline Vec3 FromBulletVec( const btVector3& v )
{
    const Vec3 r = {{v.getX(), v.getY(), v.getZ()}};
    return r;
}

void InitPhysicsManager()
{
    CollisionConfiguration = new btDefaultCollisionConfiguration();
    CollisionDispatcher = new btCollisionDispatcher(CollisionConfiguration);
    BroadphaseInterface = new btDbvtBroadphase();
    ConstraintSolver = new btSequentialImpulseConstraintSolver();
    World = new btDiscreteDynamicsWorld(CollisionDispatcher,
                                        BroadphaseInterface,
                                        ConstraintSolver,
                                        CollisionConfiguration);

    memset(Forces, 0, sizeof(Forces));

    World->setInternalTickCallback(WorldTickCallback);
}

void DestroyPhysicsManager()
{
    delete World;
    delete ConstraintSolver;
    delete CollisionDispatcher;
    delete CollisionConfiguration;
    delete BroadphaseInterface;
}

void UpdatePhysicsManager( double timeDelta )
{
    static const int maxSteps = MAX_FRAME_FREQUENCY / SIMULATION_FREQUENCY;
    static const double stepTimeDelta = 1.0 / (double)SIMULATION_FREQUENCY;
    World->stepSimulation(timeDelta, maxSteps, stepTimeDelta);
    ApplyForces(timeDelta);
}

void SetGravity( Vec3 force )
{
    World->setGravity(ToBulletVec(force));
}

static void WorldTickCallback( btDynamicsWorld* world, btScalar timeDelta )
{
    HandleCollisions();
}

static CollisionShape* CreateCollisionShape( CollisionShapeType type, btCollisionShape* bulletInstance )
{
    CollisionShape* shape = new CollisionShape;
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
    for(int i = 0; i < shapeCount; i++)
    {
        const btTransform transform(btQuaternion(),
                                    btVector3(ToBulletVec(positions[i])));
        bulletInstance->addChildShape(transform, shapes[i]->bulletInstance);
    }
    return CreateCollisionShape(COMPOUND_SHAPE, bulletInstance);
}

static void FreeCollisionShape( CollisionShape* shape )
{
    if(shape->type == COMPOUND_SHAPE)
    {
        const btCompoundShape* compound = (btCompoundShape*)shape->bulletInstance;
        const int childCount = compound->getNumChildShapes();
        for(int i = 0; i < childCount; i++)
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

Solid* CreateSolid( float mass, Vec3 position, Quat rotation, CollisionShape* shape )
{
    Solid* solid = new Solid;
    memset(solid, 0, sizeof(Solid));
    InitReferenceCounter(&solid->refCounter);

    btVector3 localInertia;
    if(shape->type == EMPTY_SHAPE)
    {
        const float v = 0.4f * mass * 0.5f; // simulate a sphere with radius 0.5
        localInertia = btVector3(v,v,v);
    }
    else
    {
        shape->bulletInstance->calculateLocalInertia(mass, localInertia);
    }

    btMotionState* motionState =
        new btDefaultMotionState(btTransform(btQuaternion(rotation._[0],
                                                          rotation._[1],
                                                          rotation._[2],
                                                          rotation._[3]),
                                             ToBulletVec(position)));

    btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass,
                                                         motionState,
                                                         shape->bulletInstance,
                                                         localInertia);

    btRigidBody* rigidBody = new btRigidBody(rigidBodyCI);

    solid->rigidBody = rigidBody;
    solid->collisionShape = shape;
    solid->collisionThreshold = INFINITE_COLLISION_THRESHOLD;
    rigidBody->setUserPointer(solid);
    World->addRigidBody(rigidBody);
    ReferenceCollisionShape(shape);

    solid->rigidBody->setGravity(btVector3(0,0,0)); // See EnableGravityForSolid

    // TEST BEGIN
    solid->rigidBody->setSleepingThresholds(0, 0);
    // TEST END

    return solid;
}

static void FreeSolid( Solid* solid )
{
    DestroyAllFocesOfSolid(solid);
    World->removeRigidBody(solid->rigidBody);
    ReleaseCollisionShape(solid->collisionShape);
    delete solid->rigidBody->getMotionState();
    delete solid->rigidBody;
    delete solid;
}

void ReferenceSolid( Solid* solid )
{
    Reference(&solid->refCounter);
}

void ReleaseSolid( Solid* solid )
{
    Release(&solid->refCounter);
    if(!HasReferences(&solid->refCounter))
        FreeSolid(solid);
}

float GetSolidMass( const Solid* solid )
{
    return 1.0f / solid->rigidBody->getInvMass();
}

void SetSolidMass( const Solid* solid, float mass )
{
    btVector3 localInertia;
    solid->collisionShape->bulletInstance->calculateLocalInertia(mass, localInertia);
    solid->rigidBody->setMassProps(mass, localInertia);
}

void SetSolidRestitution( const Solid* solid, float restitution )
{
    solid->rigidBody->setRestitution(restitution);
}

void SetSolidFriction( const Solid* solid, float friction )
{
    solid->rigidBody->setFriction(friction);
}

void SetSolidCollisionThreshold( Solid* solid, float threshold )
{
    if(threshold >= 0)
        solid->collisionThreshold = threshold;
    else
        solid->collisionThreshold = INFINITE_COLLISION_THRESHOLD;
}

Vec3 GetSolidPosition( const Solid* solid )
{
    btTransform transform;
    solid->rigidBody->getMotionState()->getWorldTransform(transform);
    const btVector3& position = transform.getOrigin();
    return FromBulletVec(position);
}

Quat GetSolidRotation( const Solid* solid )
{
    btTransform transform;
    solid->rigidBody->getMotionState()->getWorldTransform(transform);
    const btQuaternion& rotation = transform.getRotation();
    const Quat r = {{rotation.getX(),
                     rotation.getY(),
                     rotation.getZ(),
                     rotation.getW()}};
    return r;
}

Mat4 GetSolidTransformation( const Solid* solid, int copyFlags )
{
    btTransform bulletTransform;
    solid->rigidBody->getMotionState()->getWorldTransform(bulletTransform);
#if defined(BT_USE_SSE_IN_API)
    ATTRIBUTE_ALIGNED16(Mat4) transform;
#else
    Mat4 transform;
#endif
    bulletTransform.getOpenGLMatrix(transform._);
    return FilterMat4(transform, copyFlags);
}

Mat4 TryToGetSolidTransformation( const Solid* solid, int copyFlags )
{
    if(solid)
        return GetSolidTransformation(solid, copyFlags);
    else
        return Mat4Identity;
}

Vec3 GetSolidLinearVelocity( const Solid* solid )
{
    const btVector3& velocity = solid->rigidBody->getLinearVelocity();
    return FromBulletVec(velocity);
}

Vec3 GetSolidAngularVelocity( const Solid* solid )
{
    const btVector3& velocity = solid->rigidBody->getAngularVelocity();
    return FromBulletVec(velocity);
}

void EnableGravityForSolid( const Solid* solid, bool enable )
{
    const int flags = solid->rigidBody->getFlags();
    if(enable)
        solid->rigidBody->setFlags(flags & ~BT_DISABLE_WORLD_GRAVITY);
    else
        solid->rigidBody->setFlags(flags | BT_DISABLE_WORLD_GRAVITY);
}

void ApplySolidImpulse( const Solid* solid,
                        Vec3 impulse,
                        Vec3 relativePosition,
                        bool useLocalCoordinates )
{
    const bool isCentral = ArraysAreEqual(relativePosition._,
                                          Vec3Zero._,
                                          3);
    if(useLocalCoordinates)
    {
        const Mat4 rotation = GetSolidTransformation(solid, COPY_ROTATION);
        impulse          = MulMat4ByVec3(rotation, impulse);
        relativePosition = MulMat4ByVec3(rotation, relativePosition);
    }

    if(isCentral)
    {
        solid->rigidBody->applyCentralImpulse(ToBulletVec(impulse));
    }
    else
    {
        solid->rigidBody->applyImpulse(ToBulletVec(impulse),
                                       ToBulletVec(relativePosition));
    }
}

static void DestroyAllFocesOfSolid( Solid* solid )
{
    for(int i = 0; i < MAX_FORCES; i++)
    {
        Force* force = &Forces[i];
        if(force->solid == solid)
        {
            FatalError("Solid %p still had force %p on destruction.",
                       solid, force);
            DestroyForce(force);
        }
    }
}

static Force* FindUnusedForce()
{
    for(int i = 0; i < MAX_FORCES; i++)
        if(!Forces[i].solid)
            return &Forces[i];
    return NULL;
}

Force* CreateForce( Solid* solid )
{
    Force* force = FindUnusedForce();
    if(force)
    {
        memset(force, 0, sizeof(Force));
        force->solid = solid;
        return force;
    }
    else
    {
        return NULL;
    }
}

void SetForce( Force* force,
               Vec3 value,
               Vec3 relativePosition,
               bool useLocalCoordinates )
{
    force->value = value;
    force->relativePosition = relativePosition;
    force->useLocalCoordinates = useLocalCoordinates;
}

void DestroyForce( Force* force )
{
    force->solid = NULL;
}

static void ApplyForces( float timeDelta )
{
    for(int i = 0; i < MAX_FORCES; i++)
    {
        const Force* force = &Forces[i];
        if(force->solid)
        {
            Vec3 value;
            REPEAT(3,i) { value._[i] = force->value._[i] * timeDelta; }
            Vec3 relativePosition = force->relativePosition;

            const bool isCentral = ArraysAreEqual(relativePosition._,
                                                  Vec3Zero._,
                                                  3);

            if(force->useLocalCoordinates)
            {
                const Mat4 rotation = GetSolidTransformation(force->solid, COPY_ROTATION);
                value            = MulMat4ByVec3(rotation, value);
                relativePosition = MulMat4ByVec3(rotation, relativePosition);
            }

            if(isCentral)
            {
                force->solid->rigidBody->applyCentralForce(ToBulletVec(value));
            }
            else
            {
                force->solid->rigidBody->applyForce(ToBulletVec(value),
                                                    ToBulletVec(relativePosition));
            }
        }
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

static void HandleCollisions()
{
    if(!CurrentCollisionCallback)
        return;

    btDispatcher* dispatcher = World->getDispatcher();
    const int manifoldCount = dispatcher->getNumManifolds();
    for(int i = 0; i < manifoldCount; i++)
    {
        const btPersistentManifold* manifold = dispatcher->getManifoldByIndexInternal(i);

        const btCollisionObject* coA = (const btCollisionObject*)manifold->getBody0();
        const btCollisionObject* coB = (const btCollisionObject*)manifold->getBody1();
        Solid* solidA = (Solid*)coA->getUserPointer();
        Solid* solidB = (Solid*)coB->getUserPointer();

        if(!solidA || !solidB)
            FatalError("Collision occured, but user pointers are NULL!");

        const int contactCount = manifold->getNumContacts();
        for(int j = 0; j < contactCount; j++)
        {
            const btManifoldPoint& point = manifold->getContactPoint(j);
            if(PropagateCollision(point, solidA, solidB))
            {
                const btVector3& pointOnA  = point.getPositionWorldOnA();
                const btVector3& pointOnB  = point.getPositionWorldOnB();
                const btVector3& normalOnB = point.m_normalWorldOnB;

                Collision collision;
                collision.a = solidA;
                collision.b = solidB;
                collision.pointOnA  = FromBulletVec(pointOnA);
                collision.pointOnB  = FromBulletVec(pointOnB);
                collision.normalOnB = FromBulletVec(normalOnB);
                collision.impulse = point.m_appliedImpulse;

                CurrentCollisionCallback(&collision);
            }
        }
    }
}
