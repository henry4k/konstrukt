#include "../PhysicsManager.h"
#include "TestTools.h"
#include <dummy/bdd.hpp>

using namespace dummy;


class ModuleScope
{
private:
    bool destroyed;

public:
    ModuleScope() { InitPhysicsManager(); destroyed = false; }
    ~ModuleScope() { if(!destroyed) DestroyPhysicsManager(); }
    void destroy() { DestroyPhysicsManager(); destroyed = true; };
};

int main( int argc, char** argv )
{
    InitTests(argc, argv);

    Describe("Physics manager")
        .use(dummySignalSandbox)

        .it("can be initialized and destructed.", [](){

            InitPhysicsManager();
            DestroyPhysicsManager();
        });

    Describe("Collision shapes")
        .use(dummySignalSandbox)

        .it("can be referenced and released.", [](){

            ModuleScope scope;

            CollisionShape* shape = CreateSphereCollisionShape(1);
            ReferenceCollisionShape(shape);
            ReleaseCollisionShape(shape);
        });

    Describe("Solids")
        .use(dummySignalSandbox)

        .it("can be referenced and released.", [](){

            ModuleScope scope;

            CollisionShape* shape = CreateSphereCollisionShape(1);

            Solid* s1 = CreateSolid(1, Vec3Zero, QuatIdentity, shape);
            Require(s1 != NULL);

            Solid* s2 = CreateSolid(1, Vec3Zero, QuatIdentity, shape);
            Require(s2 != NULL);

            ReferenceSolid(s1);
            ReferenceSolid(s2);

            ReleaseSolid(s1);
            ReleaseSolid(s2);
        });

    return RunTests();
}
