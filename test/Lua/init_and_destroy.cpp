#include <src/Lua.h>
#include <test/TestCore.h>

int main()
{
    SetupTestCore();

    plan(3);
    ok(InitLua(), "Initialized Lua");
    ok(GetLuaState() != NULL, "Lua state is available");
    DestroyLua();
    ok(GetLuaState() == NULL, "Lua state is not available");

    TeardownTestCore();
    return 0;
}
