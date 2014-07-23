#include <string.h> // memset
#include <bullet/BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.h>
#include <bullet/BulletCollision/CollisionDispatch/btCollisionDispatcher.h>
#include <bullet/BulletCollision/BroadphaseCollision/btDbvtBroadphase.h>
#include <bullet/BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.h>
#include <bullet/BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#include <BulletDynamics/Dynamics/btRigidBody.h>

#include "Common.h"
#include "Math.h"
#include "Reference.h"
#include "OpenGL.h" // glfwGetTime
#include "PhysicsManager.h"


struct Solid
{
    bool active;
    ReferenceCounter refCounter;
    glm::vec3 position;
    Aabb* collisionShapes;
    int collisionShapeCount;
};


static const int MAX_SOLIDS = 8;
static Solid Solids[MAX_SOLIDS];

static btCollisionConfiguration* CollisionConfiguration = NULL;
static btCollisionDispatcher* CollisionDispatcher = NULL;
static btBroadphaseInterface* BroadphaseInterface = NULL;
static btConstraintSolver* ConstraintSolver = NULL;
static btDiscreteDynamicsWorld* World = NULL;

static const double MAX_STEP_TIME_DELTA = 1.0/20.0;
static double LastStepTime = 0;


static void FreeSolid( Solid* solid );

bool InitPhysicsManager()
{
    memset(Solids, 0, sizeof(Solids));

    CollisionConfiguration = new btDefaultCollisionConfiguration();
    CollisionDispatcher = new btCollisionDispatcher(CollisionConfiguration);
    BroadphaseInterface = new btDbvtBroadphase();
    ConstraintSolver = new btSequentialImpulseConstraintSolver();
    World = new btDiscreteDynamicsWorld(CollisionDispatcher,
                                        BroadphaseInterface,
                                        ConstraintSolver,
                                        CollisionConfiguration);

    LastStepTime = glfwGetTime();

    return true;
}

void DestroyPhysicsManager()
{
    for(int i = 0; i < MAX_SOLIDS; i++)
        if(Solids[i].active)
            Error("Solid #%d (%p) was still active when the manager was destroyed.",
                i, &Solids[i]);

    delete CollisionConfiguration;
    delete CollisionDispatcher;
    delete BroadphaseInterface;
    delete ConstraintSolver;
    delete World;
}

static void UpdateSolid( Solid* solid )
{
    if(!solid->active)
        return;

    // TODO: Run simulation here!
}

static void StepPhysicsManager( double timeDelta )
{
    for(int i = 0; i < MAX_SOLIDS; i++)
        UpdateSolid(&Solids[i]);

    World->stepSimulation(timeDelta, 1, MAX_STEP_TIME_DELTA);
}

void UpdatePhysicsManager()
{
    const double currentTime = glfwGetTime();
    const double timeDelta = currentTime - LastStepTime;
    const int stepCount = (int)(timeDelta / MAX_STEP_TIME_DELTA);
    for(int i = 0; i < stepCount; i++)
        StepPhysicsManager(MAX_STEP_TIME_DELTA);
    LastStepTime += MAX_STEP_TIME_DELTA * stepCount;
    // TODO: Prevent spiral of death
}

static Solid* FindInactiveSolid()
{
    for(int i = 0; i < MAX_SOLIDS; i++)
        if(!Solids[i].active)
            return &Solids[i];
    return NULL;
}

Solid* CreateSolid()
{
    Solid* solid = FindInactiveSolid();
    if(solid)
    {
        memset(solid, 0, sizeof(Solid));
        solid->active = true;
        InitReferenceCounter(&solid->refCounter);
        return solid;
    }
    else
    {
        Error("Can't create more solids.");
        return NULL;
    }
}

static void FreeSolid( Solid* solid )
{
    solid->active = false;
    FreeReferenceCounter(&solid->refCounter);
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
    return glm::vec3(); // TODO: Implement me!
}

glm::vec3 GetSolidRotation( const Solid* solid )
{
    return glm::vec3(); // TODO: Implement me!
}

glm::mat4 GetSolidTransformation( const Solid* solid )
{
    return glm::mat4(); // TODO: Implement me!
}
