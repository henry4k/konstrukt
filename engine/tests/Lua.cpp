#include <string.h> // memset
#include "../Lua.h"
#include "TestTools.h"


/**
 * Should be used inside Lua callbacks.
 */
#define LuaRequire( L, E ) LuaRequire_((L), #E, (E))
static inline void LuaRequire_( lua_State* l, const char* expression, bool result )
{
    if(!result)
        luaL_error(l, "%s failed", expression);
}

InlineTest("can be initialized and destructed")
{
    InitLua();
    Require(GetLuaState() != NULL);

    // Test if lua works
    lua_State* l = GetLuaState();
    lua_pushinteger(l, 42);
    Require(lua_tointeger(l, -1) == 42);
    lua_pop(l, 1);

    DestroyLua();
    Require(GetLuaState() == NULL);
}

static int lua_AddOne( lua_State* l )
{
    int i = luaL_checkinteger(l, 1);
    lua_pushinteger(l, i+1);
    return 1;
}

InlineTest("can register functions")
{
    InitLua();

    RegisterFunctionInLua("AddOne", lua_AddOne);

    int r = luaL_dostring(GetLuaState(), "assert(_engine.AddOne(41) == 42)");
    if(r != LUA_OK)
        dummyAbortTest(DUMMY_FAIL_TEST, "%s", lua_tostring(GetLuaState(), -1));

    DestroyLua();
}

static const char* MyDataType = "MyData";
struct MyData { int value; };

static int lua_MyData_destructor( lua_State* l )
{
    MyData* myData = (MyData*)lua_touserdata(l, 1);
    myData->value++;
    return 0;
}

InlineTest("can register and use custom data types")
{
    InitLua();
    lua_State* l = GetLuaState();

    RegisterUserDataTypeInLua(MyDataType, lua_MyData_destructor);

    // Push a new value
    MyData* originalData = (MyData*)PushUserDataToLua(l, MyDataType, sizeof(MyData));
    Require(originalData != NULL);
    originalData->value = 42;

    // Inspect value, that was pushed
    MyData* retrievedData = (MyData*)GetUserDataFromLua(l, -1, MyDataType);
    Require(retrievedData != NULL);
    Require(retrievedData == originalData);
    Require(retrievedData->value == 42);

    DestroyLua();

    Require(retrievedData->value == 43);
}

InlineTest("can push and retrieve pointers")
{
    InitLua();
    lua_State* l = GetLuaState();

    // Push a new pointer
    int data = 42;
    PushPointerToLua(l, &data);

    // Inspect pointer, that was pushed
    int* retrievedData = (int*)GetPointerFromLua(l, -1);
    Require(retrievedData == &data);

    DestroyLua();
}

InlineTest("pushes nil object instead of null pointer")
{
    InitLua();
    lua_State* l = GetLuaState();

    PushPointerToLua(l, NULL);
    Require(GetPointerFromLua(l, -1) == NULL);

    DestroyLua();
}

InlineTest("can fire events")
{
    InitLua();
    lua_State* l = GetLuaState();

    const int event = RegisterLuaEvent("MyEvent");
    Require(event != INVALID_LUA_EVENT);

    int r = luaL_dostring(l,
        "function MyEventHandler( a, b )\n"
        "    return a+b, a+c, b+c\n"
        "end\n"
        "_engine.SetEventCallback('MyEvent', MyEventHandler)\n"
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

    DestroyLua();
}

int main( int argc, char** argv )
{
    InitTests(argc, argv);
    return RunTests();
}
