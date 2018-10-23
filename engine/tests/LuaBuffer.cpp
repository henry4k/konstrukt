#include "../Lua.h"
#include "../LuaBuffer.h"
#include "TestTools.h"


char data[] = "bar";

static int Lua_Test( lua_State* l )
{
    Require(lua_gettop(l) == 9);
    Require(lua_type(l, 1) == LUA_TNIL);
    Require(lua_toboolean(l, 2) == true);
    Require(lua_tointeger(l, 3) == 42);
    Require(lua_type(l, 4) == LUA_TNUMBER);
    Require(strcmp(lua_tostring(l, 5), "foo") == 0);
    Require(strcmp(lua_tostring(l, 6), "bar") == 0);
    Require(lua_islightuserdata(l, 7));
    Require(lua_touserdata(l, 7) == data);
    Require(lua_isuserdata(l, 8));
    Require(lua_touserdata(l, 8) != data);
    Require(strncmp((const char*)lua_touserdata(l, 8), data, 3) == 0);
    Require(lua_type(l, 9) == LUA_TTABLE);
    return 0;
}

InlineTest("Basic Test")
{
    lua_State* l = luaL_newstate();
    lua_gc(l, LUA_GCSTOP, 0); // only collect manually

    LuaBuffer* buffer = CreateLuaBuffer(NATIVE_LUA_BUFFER);
    ReferenceLuaBuffer(buffer);

    AddNilToLuaBuffer(buffer);
    AddBooleanToLuaBuffer(buffer, true);
    AddIntegerToLuaBuffer(buffer, 42);
    AddNumberToLuaBuffer(buffer, 13.37);
    AddStringToLuaBuffer(buffer, "foo", 0, 0);
    AddStringToLuaBuffer(buffer, "bar", 3, 0);
    AddUserDataToLuaBuffer(buffer, data, 0);
    AddUserDataToLuaBuffer(buffer, data, 3);

    BeginListInLuaBuffer(buffer);
        AddIntegerToLuaBuffer(buffer, 111);
        BeginListInLuaBuffer(buffer);
            AddIntegerToLuaBuffer(buffer, 222);
        EndListInLuaBuffer(buffer);
        AddIntegerToLuaBuffer(buffer, 333);
    EndListInLuaBuffer(buffer);

    lua_pushcfunction(l, Lua_Test);
    const int args = PushLuaBufferToLua(buffer, l);
    lua_call(l, args, 0);

    ReleaseLuaBuffer(buffer);
    lua_close(l);
}

int main( int argc, char** argv )
{
    InitTests(argc, argv);
    return RunTests();
}
