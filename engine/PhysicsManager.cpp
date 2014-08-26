#include <string.h> // memset
#include <bullet/BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.h>
#include <bullet/BulletCollision/CollisionDispatch/btCollisionDispatcher.h>
#include <bullet/BulletCollision/BroadphaseCollision/btDbvtBroadphase.h>
#include <bullet/BulletCollision/CollisionShapes/btCollisionShape.h>
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

static const int MAX_FORCES = 8;
static Force Forces[MAX_FORCES];

struct Solid
{
    ReferenceCounter refCounter;
    btRigidBody* rigidBody;
    CollisionShape* collisionShape;
};


static btCollisionConfiguration* CollisionConfiguration = NULL;
static btCollisionDispatcher* CollisionDispatcher = NULL;
static btBroadphaseInterface* BroadphaseInterface = NULL;
static btConstraintSolver* ConstraintSolver = NULL;
static btDiscreteDynamicsWorld* World = NULL;


static void FreeSolid( Solid* solid );
static void DestroyAllFocesOfSolid( Solid* solid );
static void WorldTickCallback( btDynamicsWorld* world, btScalar timeDelta );

static inline btVector3 GlmToBulletVec( const glm::vec3& v )
{
    return btVector3(v[0], v[1], v[2]);
}

static inline glm::vec3 BulletToGlmVec( const btVector3& v )
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

    World->setGravity(btVector3(0,-0.1,0));
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

CollisionShape* CreateBoxCollisionShape( glm::vec3 halfWidth )
{
    return CreateCollisionShape(BOX_SHAPE,
                                new btBoxShape(GlmToBulletVec(halfWidth)));
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
                                    btVector3(GlmToBulletVec(positions[i])));
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
                                             GlmToBulletVec(position)));

    btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass,
                                                         motionState,
                                                         shape->bulletInstance,
                                                         localInertia);

    btRigidBody* rigidBody = new btRigidBody(rigidBodyCI);

    solid->rigidBody = rigidBody;
    solid->collisionShape = shape;
    World->addRigidBody(rigidBody);
    ReferenceCollisionShape(shape);

    // TEST BEGIN
    solid->rigidBody->setSleepingThresholds(0, 0);
    Force* force = CreateForce(solid);
    SetForce(force, glm::vec3(10,0,0), glm::vec3(0,0,0), false);
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

glm::vec3 GetSolidPosition( const Solid* solid )
{
    btTransform transform;
    solid->rigidBody->getMotionState()->getWorldTransform(transform);
    const btVector3& position = transform.getOrigin();
    return BulletToGlmVec(position);
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

void GetSolidTransformation( const Solid* solid, glm::mat4* target )
{
    btTransform transform;
    solid->rigidBody->getMotionState()->getWorldTransform(transform);
    transform.getOpenGLMatrix((float*)target);
}

glm::vec3 GetSolidLinearVelocity( const Solid* solid )
{
    const btVector3& velocity = solid->rigidBody->getLinearVelocity();
    return BulletToGlmVec(velocity);
}

glm::vec3 GetSolidAngularVelocity( const Solid* solid )
{
    const btVector3& velocity = solid->rigidBody->getAngularVelocity();
    return BulletToGlmVec(velocity);
}

void ApplySolidImpulse( const Solid* solid,
                        glm::vec3 impulse,
                        glm::vec3 relativePosition,
                        bool useLocalCoordinates )
{
    if(useLocalCoordinates)
    {
        using namespace glm;

        mat4 transformation;
        GetSolidTransformation(solid, &transformation);

        impulse          = vec3(transformation * vec4(impulse, 1));
        relativePosition = vec3(transformation * vec4(relativePosition, 1));
    }

    solid->rigidBody->applyImpulse(GlmToBulletVec(impulse),
                                   GlmToBulletVec(relativePosition));
}

/**
 * Sets a force that is applied permanently, just like gravity.
 *
 * The force is applied at every simulation step.
 */
static void SetSolidPermanentForce( const Solid* solid, glm::vec3 permanentForce )
{
    const btVector3 finalForce = World->getGravity() +
                                 GlmToBulletVec(permanentForce);
    solid->rigidBody->setGravity(finalForce);
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

static void WorldTickCallback( btDynamicsWorld* world, btScalar timeDelta )
{
    for(int i = 0; i < MAX_FORCES; i++)
    {
        const Force* force = &Forces[i];
        if(force->solid)
        {
            using namespace glm;

            vec3 value = force->value * timeDelta;
            vec3 relativePosition = force->relativePosition;

            if(force->useLocalCoordinates)
            {
                mat4 transformation;
                GetSolidTransformation(force->solid, &transformation);

                value            = vec3(transformation * vec4(value, 1));
                relativePosition = vec3(transformation * vec4(relativePosition, 1));
            }

            force->solid->rigidBody->applyForce(GlmToBulletVec(value),
                                                GlmToBulletVec(relativePosition));
        }
    }
}