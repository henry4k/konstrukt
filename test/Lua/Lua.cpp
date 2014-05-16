#include <string.h> // memset
#include <src/Lua.h>

#include <test/TestTools.h>


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


int main()
{
    InitTests();

    Describe("Lua module")
        .use(dummyExceptionSandbox)

        .it("can be initialized and destructed.", [](){

            Require(InitLua() == true);
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

            Require(RegisterFunctionInLua("testfn", lua_testfn));

            int r = luaL_dostring(GetLuaState(), "assert(Native.testfn(41) == 42)");
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

            Require(RegisterUserDataTypeInLua(MyDataType, lua_MyData_destructor));

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

        .it("can fire events.", [](){

            LuaScope luaScope;
            lua_State* l = GetLuaState();

            const int event = RegisterLuaEvent("MyEvent");
            Require(event != LUA_INVALID_EVENT);

            int r = luaL_dostring(l,
                "function MyEventHandler( a, b )\n"
                "    return a+b, a+c, b+c\n"
                "end\n"
                "Native.SetEventCallback('MyEvent', MyEventHandler)\n"
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
        })

        .it("can push arrays to Lua.", [](){

            lua_CFunction lua_testfn = []( lua_State* l ) {

                const long numbers[4] = {4000, 3000, 2000, 1000};
                PushArrayToLua(l, LUA_LONG_ARRAY, 4, numbers);
                return 1;
            };

            LuaScope luaScope;
            lua_State* l = GetLuaState();

            Require(RegisterFunctionInLua("testfn", lua_testfn));

            int r = luaL_dostring(l,
                "a = Native.testfn()\n"
                "assert(#a == 4)\n"
                "assert(a[1] == 4000)\n"
                "assert(a[2] == 3000)\n"
                "assert(a[3] == 2000)\n"
                "assert(a[4] == 1000)\n");
            if(r != LUA_OK)
                dummyAbortTest(DUMMY_FAIL_TEST, "%s", lua_tostring(GetLuaState(), -1));
        })

        .it("can retrive arrays from Lua.", [](){

            lua_CFunction lua_testfn = []( lua_State* l ) {

                LuaRequire(l, GetLuaArraySize(l, 1) == 4);

                long numberBuffer[5];

                memset(numberBuffer, 0, sizeof(long)*5);
                GetArrayFromLua(l, 1, LUA_LONG_ARRAY, 0, numberBuffer);
                LuaRequire(l, numberBuffer[0] == 0);
                LuaRequire(l, numberBuffer[1] == 0);
                LuaRequire(l, numberBuffer[2] == 0);
                LuaRequire(l, numberBuffer[3] == 0);
                LuaRequire(l, numberBuffer[4] == 0);

                memset(numberBuffer, 0, sizeof(long)*5);
                GetArrayFromLua(l, 1, LUA_LONG_ARRAY, 1, numberBuffer);
                LuaRequire(l, numberBuffer[0] == 4000);
                LuaRequire(l, numberBuffer[1] == 0);
                LuaRequire(l, numberBuffer[2] == 0);
                LuaRequire(l, numberBuffer[3] == 0);
                LuaRequire(l, numberBuffer[4] == 0);

                memset(numberBuffer, 0, sizeof(long)*5);
                GetArrayFromLua(l, 1, LUA_LONG_ARRAY, 3, numberBuffer);
                LuaRequire(l, numberBuffer[0] == 4000);
                LuaRequire(l, numberBuffer[1] == 3000);
                LuaRequire(l, numberBuffer[2] == 2000);
                LuaRequire(l, numberBuffer[3] == 0);
                LuaRequire(l, numberBuffer[4] == 0);

                memset(numberBuffer, 0, sizeof(long)*5);
                GetArrayFromLua(l, 1, LUA_LONG_ARRAY, 4, numberBuffer);
                LuaRequire(l, numberBuffer[0] == 4000);
                LuaRequire(l, numberBuffer[1] == 3000);
                LuaRequire(l, numberBuffer[2] == 2000);
                LuaRequire(l, numberBuffer[3] == 1000);
                LuaRequire(l, numberBuffer[4] == 0);

                memset(numberBuffer, 0, sizeof(long)*5);
                GetArrayFromLua(l, 1, LUA_LONG_ARRAY, 5, numberBuffer);
                LuaRequire(l, numberBuffer[0] == 4000);
                LuaRequire(l, numberBuffer[1] == 3000);
                LuaRequire(l, numberBuffer[2] == 2000);
                LuaRequire(l, numberBuffer[3] == 1000);
                LuaRequire(l, numberBuffer[4] == 0);

                return 0;
            };

            LuaScope luaScope;
            lua_State* l = GetLuaState();

            Require(RegisterFunctionInLua("testfn", lua_testfn));

            int r = luaL_dostring(l,
                "Native.testfn({4000, 3000, 2000, 1000})\n");
            if(r != LUA_OK)
                dummyAbortTest(DUMMY_FAIL_TEST, "%s", lua_tostring(GetLuaState(), -1));
        });

    return RunTests();
}
