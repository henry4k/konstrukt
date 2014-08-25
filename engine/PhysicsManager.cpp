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

    World->setGravity(btVector3(0,-0.1,0));

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
                                new btBoxShape(btVector3(halfWidth[0],
                                                         halfWidth[1],
                                                         halfWidth[2])));
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
                                    btVector3(positions[i][0],
                                              positions[i][1],
                                              positions[i][2]));
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
    InitReferenceCounter(&solid->refCounter);

    btVector3 localInertia;
    shape->bulletInstance->calculateLocalInertia(mass, localInertia);

    btMotionState* motionState =
        new btDefaultMotionState(btTransform(btQuaternion(rotation[0],
                                                          rotation[1],
                                                          rotation[2],
                                                          rotation[3]),
                                             btVector3(position[0],
                                                       position[1],
                                                       position[2])));

    btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass,
                                                         motionState,
                                                         shape->bulletInstance,
                                                         localInertia);

    btRigidBody* rigidBody = new btRigidBody(rigidBodyCI);

    solid->rigidBody = rigidBody;
    solid->collisionShape = shape;
    World->addRigidBody(rigidBody);
    ReferenceCollisionShape(shape);

    return solid;
}

static void FreeSolid( Solid* solid )
{
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
    return glm::vec3(position.getX(),
                     position.getY(),
                     position.getZ());
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
    return glm::vec3(velocity.getX(),
                     velocity.getY(),
                     velocity.getZ());
}

glm::vec3 GetSolidAngularVelocity( const Solid* solid )
{
    const btVector3& velocity = solid->rigidBody->getAngularVelocity();
    return glm::vec3(velocity.getX(),
                     velocity.getY(),
                     velocity.getZ());
}

void SetSolidPermanentForce( const Solid* solid, glm::vec3 permanentForce )
{
    const btVector3 finalForce = World->getGravity() +
                                 btVector3(permanentForce[0],
                                           permanentForce[1],
                                           permanentForce[2]);
    solid->rigidBody->setGravity(finalForce);
}

void ApplySolidImpulse( const Solid* solid, glm::vec3 impulse, glm::vec3 relativePosition )
{
    solid->rigidBody->applyImpulse(btVector3(impulse[0],
                                             impulse[1],
                                             impulse[2]),
                                   btVector3(relativePosition[0],
                                             relativePosition[1],
                                             relativePosition[2]));
}