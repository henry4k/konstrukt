#include "../Lua.h"
#include "../RenderManager.h"
#include "RenderManager.h"


static int Lua_GetFrameTime( lua_State* l )
{
    lua_pushnumber(l, GetFrameTime());
    return 1;
}

void RegisterRenderManagerInLua()
{
    RegisterFunctionInLua("GetFrameTime", Lua_GetFrameTime);
}
