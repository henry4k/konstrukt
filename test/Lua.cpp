#include <src/Lua.h>

#include "TestTools.h"


class LuaScope
{
private:
    bool destroyed;

public:
    LuaScope() { InitLua(); destroyed = false; }
    ~LuaScope() { if(!destroyed) DestroyLua(); }
    void destroy() { DestroyLua(); destroyed = true; };
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

            lua_CFunction lua_testfn = []( lua_State* l ) {
                int i = luaL_checkinteger(l, 1);
                lua_pushinteger(l, i+1);
                return 1;
            };

            LuaScope luaScope;

            RegisterFunctionInLua("testfn", lua_testfn);
            int r = luaL_dostring(GetLuaState(), "assert(testfn(41) == 42)");
            if(r != LUA_OK)
                dummyAbortTest(DUMMY_FAIL_TEST, "%s", lua_tostring(GetLuaState(), -1));
        })

        .it("can register data types.", [](){

            static const char* MyDataType = "MyData";

            struct MyData
            {
                int value;
            };

            lua_CFunction lua_MyData_destructor = []( lua_State* l ){

                MyData* myData = reinterpret_cast<MyData*>(lua_touserdata(l, 1));
                myData->value++;
                return 0;
            };

            LuaScope luaScope;
            lua_State* l = GetLuaState();

            RegisterUserDataTypeInLua(MyDataType, lua_MyData_destructor);

            // Push a new value
            MyData* originalData = reinterpret_cast<MyData*>(
                PushUserDataToLua(l, MyDataType, sizeof(MyData)));
            Require(originalData != NULL);
            originalData->value = 42;

            // Inspect value, that was pushed
            MyData* retrievedData = reinterpret_cast<MyData*>(
                GetUserDataFromLua(l, -1, MyDataType));
            Require(retrievedData != NULL);
            Require(retrievedData == originalData);
            Require(retrievedData->value == 42);

            luaScope.destroy();

            Require(retrievedData->value == 43);
        });

    return RunTests();
}
