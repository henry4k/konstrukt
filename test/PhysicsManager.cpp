#include <src/PhysicsManager.h>
#include <test/TestTools.h>

class ModuleScope
{
private:
    bool destroyed;

public:
    ModuleScope() { InitPhysicsManager(); destroyed = false; }
    ~ModuleScope() { if(!destroyed) DestroyPhysicsManager(); }
    void destroy() { DestroyPhysicsManager(); destroyed = true; };
};

int main()
{
    InitTests();

    Describe("Physics manager")
        .use(dummyExceptionSandbox)

        .it("can be initialized and destructed.", [](){

            Require(InitPhysicsManager() == true);
            DestroyPhysicsManager();
        });

    Describe("Physics objects")
        .use(dummyExceptionSandbox)

        .it("can be created and freed.", [](){

            ModuleScope scope;

            PhysicsObject* o1 = CreatePhysicsObject();
            Require(o1 != NULL);
            Require(o1->active);

            PhysicsObject* o2 = CreatePhysicsObject();
            Require(o2 != NULL);
            Require(o2->active);

            Require(o1 != o2);

            FreePhysicsObject(o1);
            Require(!o1->active);
            FreePhysicsObject(o2);
            Require(!o2->active);
        });

    return RunTests();
}
