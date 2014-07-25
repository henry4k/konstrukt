#include <string.h> // memset
#include <bullet/BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.h>
#include <bullet/BulletCollision/CollisionDispatch/btCollisionDispatcher.h>
#include <bullet/BulletCollision/BroadphaseCollision/btDbvtBroadphase.h>
#include <bullet/BulletCollision/CollisionShapes/btCollisionShape.h>
#include <bullet/BulletCollision/CollisionShapes/btBoxShape.h>
#include <bullet/BulletCollision/CollisionShapes/btSphereShape.h>
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

struct CollisionShape
{
    ReferenceCounter refCounter;
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

static CollisionShape* CreateCollisionShape( btCollisionShape* bulletInstance )
{
    CollisionShape* shape = new CollisionShape;
    InitReferenceCounter(&shape->refCounter);
    shape->bulletInstance = bulletInstance;
    shape->bulletInstance->setUserPointer(shape);
    return shape;
}

CollisionShape* CreateBoxCollisionShape( glm::vec3 halfWidth )
{
    return CreateCollisionShape(new btBoxShape(btVector3(halfWidth[0],
                                                         halfWidth[1],
                                                         halfWidth[2])));
}

CollisionShape* CreateSphereCollisionShape( float radius )
{
    return CreateCollisionShape(new btSphereShape(radius));
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
    return CreateCollisionShape(bulletInstance);
}

/*
static CollisionShape* BulletInstanceToCollisionShape( btCollisionShape* bulletInstance )
{
    return (CollisionShape*)bulletInstance->getUserPointer();
}
*/

static void FreeCollisionShape( CollisionShape* shape )
{
    // TODO:
    // if(shape->bulletInstance instanceof btCompoundCollisionShape)
    //     for(childs in bulletInstance)
    //         FreeCollisionShape(child)
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

Solid* CreateSolid( CollisionShape* shape )
{
    Solid* solid = new Solid;
    InitReferenceCounter(&solid->refCounter);

    btMotionState* motionState =
        new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1),
                                             btVector3(0, -1, 0)));

    btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(1,
                                                         motionState,
                                                         shape->bulletInstance,
                                                         btVector3(0,0,0));

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

glm::vec3 GetSolidPosition( const Solid* solid )
{
    btTransform transform;
    solid->rigidBody->getMotionState()->getWorldTransform(transform);
    const btVector3& position = transform.getOrigin();
    return glm::vec3(position.getX(),
                     position.getY(),
                     position.getZ());
}

glm::vec3 GetSolidRotation( const Solid* solid )
{
    /*
    btTransform transform;
    solid->rigidBody->getMotionState()->getWorldTransform(transform);
    const btQuaternion& rotation = transform->getRotation();
    return glm::quat(rotation.getX(),
                     rotation.getY(),
                     rotation.getZ(),
                     rotation.getW());
    */
    FatalError("Not yet implemented!");
    return glm::vec3(); // TODO: Implement quaternion support!
}

void GetSolidTransformation( const Solid* solid, glm::mat4* target )
{
    btTransform transform;
    solid->rigidBody->getMotionState()->getWorldTransform(transform);
    transform.getOpenGLMatrix((float*)target);
}
