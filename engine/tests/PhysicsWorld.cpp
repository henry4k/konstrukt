#include "../PhysicsWorld.h"
#include "TestTools.h"
#include <dummy/bdd.hpp>

using namespace dummy;


int main( int argc, char** argv )
{
    InitTests(argc, argv);

    Describe("Physics world")
        .use(dummySignalSandbox)

        .it("can be initialized and destructed.", [](){

            PhysicsWorld* world = CreatePhysicsWorld();
            DestroyPhysicsWorld(world);
        });

    Describe("Collision shapes")
        .use(dummySignalSandbox)

        .it("can be referenced and released.", [](){

            CollisionShape* shape = CreateSphereCollisionShape(1);
            ReferenceCollisionShape(shape);
            ReleaseCollisionShape(shape);
        });

    Describe("Solids")
        .use(dummySignalSandbox)

        .it("can be referenced and released.", [](){

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
        });

    return RunTests();
}
