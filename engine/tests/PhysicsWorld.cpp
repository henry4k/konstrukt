#include "../PhysicsWorld.h"
#include "TestTools.h"


InlineTest("PhysicsWorld can be initialized and destructed.")
{
    PhysicsWorld* world = CreatePhysicsWorld();
    DestroyPhysicsWorld(world);
}

InlineTest("Collision shapes can be referenced and released.")
{
    CollisionShape* shape = CreateSphereCollisionShape(1);
    ReferenceCollisionShape(shape);
    ReleaseCollisionShape(shape);
}

InlineTest("Solids can be referenced and released.")
{
    PhysicsWorld* world = CreatePhysicsWorld();

    CollisionShape* shape = CreateSphereCollisionShape(1);
    ReferenceCollisionShape(shape);

    SolidProperties properties;
    properties.mass = 1;
    properties.restitution = 1;
    properties.friction = 1;
    properties.collisionThreshold = 1;
    properties.affectedByGravity = true;

    SolidMotionState motionState;
    motionState.position = Vec3Zero;
    motionState.rotation = QuatIdentity;
    motionState.linearVelocity = Vec3Zero;
    motionState.angularVelocity = Vec3Zero;

    SolidId s1 = CreateSolid(world, &properties, &motionState, shape);
    SolidId s2 = CreateSolid(world, &properties, &motionState, shape);

    ReferenceSolid(world, s1);
    ReferenceSolid(world, s2);

    ReleaseSolid(world, s1);
    ReleaseSolid(world, s2);

    ReleaseCollisionShape(shape);
    DestroyPhysicsWorld(world);
}

int main( int argc, char** argv )
{
    InitTests(argc, argv);
    return RunTests();
}
