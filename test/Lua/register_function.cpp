#include <src/Lua.h>
#include <test/TestCore.h>

bool FunctionWasCalled = false;

int Lua_testfn( lua_State* l )
{
    FunctionWasCalled = true;
    int i = luaL_checkinteger(l, 1);
    lua_pushinteger(l, i+1);
    return 1;
}

int main()
{
    SetupTestCore();

    plan(2);

    InitLua();
    RegisterFunctionInLua("testfn", Lua_testfn);

    int r = luaL_dostring(GetLuaState(), "assert(testfn(41) == 42)");
    ok(r == LUA_OK, "Script executed without error");
    if(r != LUA_OK)
        diag(lua_tostring(GetLuaState(), -1));

    ok(FunctionWasCalled, "C function was called from lua");

    DestroyLua();

    TeardownTestCore();
    return 0;
}
