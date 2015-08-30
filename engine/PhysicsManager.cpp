#include <string.h> // memset, memcpy

#include <bullet/BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.h>
#include <bullet/BulletCollision/CollisionDispatch/btCollisionDispatcher.h>
#include <bullet/BulletCollision/BroadphaseCollision/btDbvtBroadphase.h>
#include <bullet/BulletCollision/CollisionShapes/btCollisionShape.h>
#include <bullet/BulletCollision/CollisionShapes/btEmptyShape.h>
#include <bullet/BulletCollision/CollisionShapes/btBoxShape.h>
#include <bullet/BulletCollision/CollisionShapes/btSphereShape.h>
#include <bullet/BulletCollision/CollisionShapes/btCapsuleShape.h>
#include <bullet/BulletCollision/CollisionShapes/btCompoundShape.h>
#include <bullet/BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.h>
#include <bullet/BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#include <bullet/BulletDynamics/Dynamics/btRigidBody.h>
#include <bullet/LinearMath/btDefaultMotionState.h>

#include "Common.h"
#include "Game.h" // SIMULATION_FREQUENCY, MAX_FRAME_FREQUENCY
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
    glm::vec3 value;
    glm::vec3 relativePosition;
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

static inline btVector3 BulletFromGlmVec( const glm::vec3& v )
{
    return btVector3(v[0], v[1], v[2]);
}

static inline glm::vec3 GlmFromBulletVec( const btVector3& v )
{
    return glm::vec3(v.getX(), v.getY(), v.getZ());
}

bool InitPhysicsManager()
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

    return true;
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

void SetGravity( glm::vec3 force )
{
    World->setGravity(BulletFromGlmVec(force));
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

CollisionShape* CreateBoxCollisionShape( glm::vec3 halfWidth )
{
    return CreateCollisionShape(BOX_SHAPE,
                                new btBoxShape(BulletFromGlmVec(halfWidth)));
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

CollisionShape* CreateCompoundCollisionShape( int shapeCount, CollisionShape** shapes, glm::vec3* positions )
{
    btCompoundShape* bulletInstance = new btCompoundShape(false);
    for(int i = 0; i < shapeCount; i++)
    {
        const btTransform transform(btQuaternion(),
                                    btVector3(BulletFromGlmVec(positions[i])));
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

Solid* CreateSolid( float mass, glm::vec3 position, glm::quat rotation, CollisionShape* shape )
{
    Solid* solid = new Solid;
    memset(solid, 0, sizeof(Solid));
    InitReferenceCounter(&solid->refCounter);

    btVector3 localInertia;
    shape->bulletInstance->calculateLocalInertia(mass, localInertia);

    btMotionState* motionState =
        new btDefaultMotionState(btTransform(btQuaternion(rotation[0],
                                                          rotation[1],
                                                          rotation[2],
                                                          rotation[3]),
                                             BulletFromGlmVec(position)));

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

glm::vec3 GetSolidPosition( const Solid* solid )
{
    btTransform transform;
    solid->rigidBody->getMotionState()->getWorldTransform(transform);
    const btVector3& position = transform.getOrigin();
    return GlmFromBulletVec(position);
}

glm::quat GetSolidRotation( const Solid* solid )
{
    btTransform transform;
    solid->rigidBody->getMotionState()->getWorldTransform(transform);
    const btQuaternion& rotation = transform.getRotation();
    return glm::quat(rotation.getX(),
                     rotation.getY(),
                     rotation.getZ(),
                     rotation.getW());
}

void GetSolidTransformation( const Solid* solid, int copyFlags, glm::mat4* target )
{
    btTransform transform;
    solid->rigidBody->getMotionState()->getWorldTransform(transform);
#if defined(BT_USE_SSE_IN_API)
    ATTRIBUTE_ALIGNED16(glm::mat4) alignedTarget;
    memcpy(&alignedTarget, target, sizeof(alignedTarget));
    transform.getOpenGLMatrix((float*)&alignedTarget);
    memcpy(target, &alignedTarget, sizeof(alignedTarget));
#else
    transform.getOpenGLMatrix((float*)target);
#endif
    *target = GetTransformation(*target, copyFlags);
}

glm::vec3 GetSolidLinearVelocity( const Solid* solid )
{
    const btVector3& velocity = solid->rigidBody->getLinearVelocity();
    return GlmFromBulletVec(velocity);
}

glm::vec3 GetSolidAngularVelocity( const Solid* solid )
{
    const btVector3& velocity = solid->rigidBody->getAngularVelocity();
    return GlmFromBulletVec(velocity);
}

void EnableGravityForSolid( const Solid* solid, bool enable )
{
    const int flags = solid->rigidBody->getFlags();
    if(enable)
        solid->rigidBody->setFlags(flags & ~BT_DISABLE_WORLD_GRAVITY);
    else
        solid->rigidBody->setFlags(flags | BT_DISABLE_WORLD_GRAVITY);
}

static const glm::vec3 centralPosition(0,0,0);

void ApplySolidImpulse( const Solid* solid,
                        glm::vec3 impulse,
                        glm::vec3 relativePosition,
                        bool useLocalCoordinates )
{
    const bool isCentral = relativePosition == centralPosition;
    if(useLocalCoordinates)
    {
        using namespace glm;
        mat4 rotation;
        GetSolidTransformation(solid,
                               COPY_ROTATION,
                               &rotation);

        impulse          = vec3(rotation * vec4(impulse, 1));
        relativePosition = vec3(rotation * vec4(relativePosition, 1));
    }

    if(isCentral)
    {
        solid->rigidBody->applyCentralImpulse(BulletFromGlmVec(impulse));
    }
    else
    {
        solid->rigidBody->applyImpulse(BulletFromGlmVec(impulse),
                                       BulletFromGlmVec(relativePosition));
    }
}

static void DestroyAllFocesOfSolid( Solid* solid )
{
    for(int i = 0; i < MAX_FORCES; i++)
    {
        Force* force = &Forces[i];
        if(force->solid == solid)
        {
            Error("Solid %p still had force %p on destruction. This may lead to further errors!",
                  solid,
                  force);
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
               glm::vec3 value,
               glm::vec3 relativePosition,
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
            using namespace glm;

            vec3 value = force->value * timeDelta;
            vec3 relativePosition = force->relativePosition;
            const bool isCentral = relativePosition == centralPosition;

            if(force->useLocalCoordinates)
            {
                mat4 rotation;
                GetSolidTransformation(force->solid,
                                       COPY_ROTATION,
                                       &rotation);

                value            = vec3(rotation * vec4(value, 1));
                relativePosition = vec3(rotation * vec4(relativePosition, 1));
            }

            if(isCentral)
            {
                force->solid->rigidBody->applyCentralForce(BulletFromGlmVec(value));
            }
            else
            {
                force->solid->rigidBody->applyForce(BulletFromGlmVec(value),
                                                    BulletFromGlmVec(relativePosition));
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

        if(solidA && solidB)
        {
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
                    collision.pointOnA  = GlmFromBulletVec(pointOnA);
                    collision.pointOnB  = GlmFromBulletVec(pointOnB);
                    collision.normalOnB = GlmFromBulletVec(normalOnB);
                    collision.impulse = point.m_appliedImpulse;

                    CurrentCollisionCallback(&collision);
                }
            }
        }
        else
        {
            Error("Collision occured, but user pointers are NULL!");
        }
    }
}
