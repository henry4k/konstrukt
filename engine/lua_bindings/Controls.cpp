#include "../Lua.h"
#include "../Controls.h"


const char* CONTROL_ACTION_EVENT_NAME = "ControlAction";

int g_ControlActionEvent = INVALID_LUA_EVENT;


void OnLuaControlAction( const char* name, float absolute, float delta, void* context)
{
    lua_State* l = GetLuaState();
    lua_pushstring(l, name);
    lua_pushnumber(l, absolute);
    lua_pushnumber(l, delta);
    FireLuaEvent(l, g_ControlActionEvent, 3, false);
}

int Lua_RegisterControl( lua_State* l )
{
    const char* name = luaL_checkstring(l, 1);

    const bool success = RegisterControl(name, OnLuaControlAction, NULL);
    lua_pushboolean(l, success);
    return 1;
}

bool RegisterControlsInLua()
{
    g_ControlActionEvent = RegisterLuaEvent(CONTROL_ACTION_EVENT_NAME);
    if(g_ControlActionEvent == INVALID_LUA_EVENT)
        return false;

    return
        RegisterFunctionInLua("RegisterControl", Lua_RegisterControl);
}
