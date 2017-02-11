#include "../Lua.h"
#include "../Controls.h"


const char* CONTROL_ACTION_EVENT_NAME = "ControlAction";

static int g_ControlActionEvent = INVALID_LUA_EVENT;


static void OnLuaControlAction( const char* name, float absolute, float delta, void* context)
{
    lua_State* l = GetLuaState();
    lua_pushstring(l, name);
    lua_pushnumber(l, absolute);
    lua_pushnumber(l, delta);
    FireLuaEvent(l, g_ControlActionEvent, 3, false);
}

static int Lua_RegisterControl( lua_State* l )
{
    const char* name = luaL_checkstring(l, 1);

    const bool success = RegisterControl(name, OnLuaControlAction, NULL);
    lua_pushboolean(l, success);
    return 1;
}

void RegisterControlsInLua()
{
    g_ControlActionEvent = RegisterLuaEvent(CONTROL_ACTION_EVENT_NAME);

    RegisterFunctionInLua("RegisterControl", Lua_RegisterControl);
}
