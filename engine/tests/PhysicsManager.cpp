#include <engine/PhysicsManager.h>
#include "TestTools.h"

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
        .use(dummyExceptionSandbox)

        .it("can be initialized and destructed.", [](){

            Require(InitPhysicsManager() == true);
            DestroyPhysicsManager();
        });

    Describe("Collision shapes")
        .use(dummyExceptionSandbox)

        .it("can be referenced and released.", [](){

            ModuleScope scope;

            CollisionShape* shape = CreateSphereCollisionShape(1);
            ReferenceCollisionShape(shape);
            ReleaseCollisionShape(shape);
        });

    Describe("Solids")
        .use(dummyExceptionSandbox)

        .it("can be referenced and released.", [](){

            ModuleScope scope;

            CollisionShape* shape = CreateSphereCollisionShape(1);

            Solid* s1 = CreateSolid(1, glm::vec3(), glm::quat(), shape);
            Require(s1 != NULL);

            Solid* s2 = CreateSolid(1, glm::vec3(), glm::quat(), shape);
            Require(s2 != NULL);

            ReferenceSolid(s1);
            ReferenceSolid(s2);

            ReleaseSolid(s1);
            ReleaseSolid(s2);
        });

    return RunTests();
}
