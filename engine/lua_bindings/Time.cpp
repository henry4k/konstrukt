#include "../Lua.h"
#include "../Time.h"
#include "Time.h"


static int Lua_GetTime( lua_State* l )
{
    lua_pushnumber(l, GetTime());
    return 1;
}

bool RegisterTimeInLua()
{
    return
        RegisterFunctionInLua("GetTime", Lua_GetTime);
}
