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

    Describe("Solids")
        .use(dummyExceptionSandbox)

        .it("can be created and freed.", [](){

            ModuleScope scope;

            Solid* s1 = CreateSolid();
            Require(s1 != NULL);

            Solid* s2 = CreateSolid();
            Require(s2 != NULL);

            FreeSolid(s1);
            FreeSolid(s2);
        });

    return RunTests();
}
