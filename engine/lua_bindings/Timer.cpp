#include "../Lua.h"
#include "../OpenGL.h" // glfwGetTime
#include "Timer.h"


static int Lua_GetTime( lua_State* l )
{
    lua_pushnumber(l, glfwGetTime());
    return 1;
}

bool RegisterTimerInLua()
{
    return
        RegisterFunctionInLua("GetTime", Lua_GetTime);
}
