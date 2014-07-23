#include <string.h> // memset
#include <bullet/BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.h>
#include <bullet/BulletCollision/CollisionDispatch/btCollisionDispatcher.h>
#include <bullet/BulletCollision/BroadphaseCollision/btDbvtBroadphase.h>
#include <bullet/BulletCollision/CollisionShapes/btCollisionShape.h>
#include <bullet/BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.h>
#include <bullet/BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#include <bullet/BulletDynamics/Dynamics/btRigidBody.h>
#include <bullet/LinearMath/btDefaultMotionState.h>

#include "Common.h"
#include "Game.h" // SIMULATION_FREQUENCY, MAX_FRAME_FREQUENCY
#include "Math.h"
#include "Reference.h"
#include "PhysicsManager.h"


struct Solid
{
    ReferenceCounter refCounter;
    btRigidBody* rigidBody;
    btCollisionShape* customCollisionShape;
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

    return true;
}

void DestroyPhysicsManager()
{
    delete CollisionConfiguration;
    delete CollisionDispatcher;
    delete BroadphaseInterface;
    delete ConstraintSolver;
    delete World;
}

void UpdatePhysicsManager( double timeDelta )
{
    static const int maxSteps = MAX_FRAME_FREQUENCY / SIMULATION_FREQUENCY;
    static const double stepTimeDelta = 1.0 / (double)SIMULATION_FREQUENCY;
    World->stepSimulation(timeDelta, maxSteps, stepTimeDelta);
}

Solid* CreateSolid()
{
    Solid* solid = new Solid;
    InitReferenceCounter(&solid->refCounter);

    btCollisionShape* collisionShape = NULL; // new btSphereShape(1);

    btMotionState* motionState =
        new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1),
                                             btVector3(0, -1, 0)));

    btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(0,
                                                         motionState,
                                                         collisionShape,
                                                         btVector3(0,0,0));

    btRigidBody* rigidBody = new btRigidBody(rigidBodyCI);

    solid->rigidBody = rigidBody;
    World->addRigidBody(rigidBody);

    return solid;
}

static void FreeSolid( Solid* solid )
{
    World->removeRigidBody(solid->rigidBody);

    if(solid->customCollisionShape)
        delete solid->customCollisionShape;
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
