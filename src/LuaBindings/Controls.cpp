#include "../Lua.h"
#include "../Controls.h"


const char* KEY_CONTROL_ACTION_EVENT_NAME = "KeyControlAction";
const char* AXIS_CONTROL_ACTION_EVENT_NAME = "AxisControlAction";

int g_KeyControlActionEvent = LUA_INVALID_EVENT;
int g_AxisControlActionEvent = LUA_INVALID_EVENT;

void OnLuaKeyControlAction( const char* name, bool pressed, void* context )
{
    lua_State* l = GetLuaState();
    lua_pushstring(l, name);
    lua_pushboolean(l, pressed);
    FireLuaEvent(l, g_KeyControlActionEvent, 2, false);
}

int Lua_RegisterKeyControl( lua_State* l )
{
    const char* name = luaL_checkstring(l, 1);

    const bool success = RegisterKeyControl(name, OnLuaKeyControlAction, NULL, NULL);
    lua_pushboolean(l, success);
    return 1;
}

void OnLuaAxisControlAction( const char* name, float absolute, float delta, void* context)
{
    lua_State* l = GetLuaState();
    lua_pushstring(l, name);
    lua_pushnumber(l, absolute);
    lua_pushnumber(l, delta);
    FireLuaEvent(l, g_AxisControlActionEvent, 3, false);
}

int Lua_RegisterAxisControl( lua_State* l )
{
    const char* name = luaL_checkstring(l, 1);

    const bool success = RegisterAxisControl(name, OnLuaAxisControlAction, NULL, NULL);
    lua_pushboolean(l, success);
    return 1;
}

AutoRegisterInLua()
{
    g_KeyControlActionEvent = RegisterLuaEvent(KEY_CONTROL_ACTION_EVENT_NAME);
    if(g_KeyControlActionEvent == LUA_INVALID_EVENT)
        return false;

    g_AxisControlActionEvent = RegisterLuaEvent(AXIS_CONTROL_ACTION_EVENT_NAME);
    if(g_AxisControlActionEvent == LUA_INVALID_EVENT)
        return false;

    return
        RegisterFunctionInLua("RegisterKeyControl", Lua_RegisterKeyControl) &&
        RegisterFunctionInLua("RegisterAxisControl", Lua_RegisterAxisControl);
}
