#include "../Common.h"
#include "../Lua.h"
#include "../LuaBuffer.h"
#include "../Controls.h"


static void OnLuaControlAction( const char* name, float absolute, float delta, void* context)
{
    LuaEventListener listener = *(LuaEventListener*)context;
    LuaBuffer* buffer = BeginLuaEvent(listener);
    AddStringToLuaBuffer(buffer, name, 0, LUA_BUFFER_LITERAL_STRING);
    AddNumberToLuaBuffer(buffer, absolute);
    AddNumberToLuaBuffer(buffer, delta);
    CompleteLuaEvent(listener);
}

static int Lua_RegisterControl( lua_State* l )
{
    const char* name = luaL_checkstring(l, 1);

    LuaEventListener* listener = NEW(LuaEventListener);
    // TODO: This allocation is *never* freed!
    // The whole control system needs to be reworked soon anyway.
    *listener = GetLuaEventListener(l, name);

    const bool success = RegisterControl(name, OnLuaControlAction, listener);
    lua_pushboolean(l, success);
    return 1;
}

void RegisterControlsInLua()
{
    RegisterFunctionInLua("RegisterControl", Lua_RegisterControl);
}
