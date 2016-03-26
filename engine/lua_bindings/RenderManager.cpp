#include "../Lua.h"
#include "../RenderManager.h"
#include "RenderManager.h"


static int Lua_GetFrameTime( lua_State* l )
{
    lua_pushnumber(l, GetFrameTime());
    return 1;
}

bool RegisterRenderManagerInLua()
{
    return
        RegisterFunctionInLua("GetFrameTime", Lua_GetFrameTime);
}
