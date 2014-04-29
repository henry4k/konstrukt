#include <src/Lua.h>

#include "TestTools.h"


class LuaScope
{
public:
    LuaScope() { InitLua(); }
    ~LuaScope() { DestroyLua(); }
};

int main()
{
    InitTests();

    Describe("Lua module")

        .it("can be initialized and destructed.", [](){

            Require(InitLua() == true);
            Require(GetLuaState() != NULL);
            DestroyLua();
            Require(GetLuaState() == NULL);
        })

        .it("can register functions.", [](){

            dummyMarkTestAsTodo(NULL);

            bool functionWasCalled = false;
            lua_CFunction lua_testfn = []( lua_State* l ) {
                //functionWasCalled = true;
                int i = luaL_checkinteger(l, 1);
                lua_pushinteger(l, i+1);
                return 1;
            };

            LuaScope luaScope;

            RegisterFunctionInLua("testfn", lua_testfn);
            int r = luaL_dostring(GetLuaState(), "assert(testfn(41) == 42)");
            if(r != LUA_OK)
                dummyAbortTest(DUMMY_FAIL_TEST, "%s", lua_tostring(GetLuaState(), -1));
            Require(functionWasCalled);
        });

    return RunTests();
}
