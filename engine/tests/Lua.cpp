#include <string.h> // memset
#include "../Lua.h"
#include "TestTools.h"
#include <dummy/bdd.hpp>

using namespace dummy;


/**
 * Should be used inside Lua callbacks.
 */
#define LuaRequire( L, E ) LuaRequire_((L), #E, (E))
static inline void LuaRequire_( lua_State* l, const char* expression, bool result )
{
    if(!result)
        luaL_error(l, "%s failed", expression);
}

class LuaScope
{
private:
    bool destroyed;

public:
    LuaScope() { InitLua(); destroyed = false; }
    ~LuaScope() { if(!destroyed) DestroyLua(); }
    void destroy() { DestroyLua(); destroyed = true; };
};


int main( int argc, char** argv )
{
    InitTests(argc, argv);

    Describe("Lua module")
        .use(dummySignalSandbox)

        .it("can be initialized and destructed.", [](){

            InitLua();
            Require(GetLuaState() != NULL);

            // Test if lua works
            lua_State* l = GetLuaState();
            lua_pushinteger(l, 42);
            Require(lua_tointeger(l, -1) == 42);
            lua_pop(l, 1);

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

            int r = luaL_dostring(GetLuaState(), "assert(NATIVE.testfn(41) == 42)");
            if(r != LUA_OK)
                dummyAbortTest(DUMMY_FAIL_TEST, "%s", lua_tostring(GetLuaState(), -1));
        })

        .it("can register and use custom data types.", [](){

            static const char* MyDataType = "MyData";
            struct MyData { int value; };

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
        })

        .it("can push and retrieve pointers.", [](){

            LuaScope luaScope;
            lua_State* l = GetLuaState();

            // Push a new pointer
            int data = 42;
            PushPointerToLua(l, &data);

            // Inspect pointer, that was pushed
            int* retrievedData = reinterpret_cast<int*>(
                GetPointerFromLua(l, -1));
            Require(retrievedData == &data);

            luaScope.destroy();
        })

        .it("pushes nil object instead of null pointer.", [](){

            LuaScope luaScope;
            lua_State* l = GetLuaState();

            PushPointerToLua(l, NULL);
            Require(GetPointerFromLua(l, -1) == NULL);

            luaScope.destroy();
        })

        .it("can run script files.", [](){

            LuaScope luaScope;
            lua_State* l = GetLuaState();

            RunLuaScript(l, "data/Test.lua");
        })

        .it("can fire events.", [](){

            LuaScope luaScope;
            lua_State* l = GetLuaState();

            const int event = RegisterLuaEvent("MyEvent");
            Require(event != INVALID_LUA_EVENT);

            int r = luaL_dostring(l,
                "function MyEventHandler( a, b )\n"
                "    return a+b, a+c, b+c\n"
                "end\n"
                "NATIVE.SetEventCallback('MyEvent', MyEventHandler)\n"
                "c = 1\n");
            if(r != LUA_OK)
                dummyAbortTest(DUMMY_FAIL_TEST, "%s", lua_tostring(GetLuaState(), -1));

            lua_pushinteger(l, 10);
            lua_pushinteger(l, 20);
            const int resultCount = FireLuaEvent(l, event, 2, true);
            Require(resultCount == 3);
            Require(lua_tointeger(l, -3) == 30);
            Require(lua_tointeger(l, -2) == 11);
            Require(lua_tointeger(l, -1) == 21);
        });

    return RunTests();
}
