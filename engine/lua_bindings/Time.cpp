#include "../Common.h"
#include "../Lua.h"
#include "../LuaBuffer.h"
#include "../Time.h"
#include "Time.h"


static int Lua_GetTime( lua_State* l )
{
    lua_pushnumber(l, GetTime());
    return 1;
}

static void OnLuaTimerTriggered( Timer* timer, double timeDelta, void* context )
{
    const LuaEventListener* listener = (LuaEventListener*)context;
    LuaBuffer* buffer = BeginLuaEvent(*listener);
    AddUserDataToLuaBuffer(buffer, timer, 0);
    AddNumberToLuaBuffer(buffer, timeDelta);
    CompleteLuaEvent(*listener);
}

struct LuaTimer
{
    Timer* timer;
    LuaEventListener listener;
};

static int Lua_CreateTimer( lua_State* l )
{
    const double minDelay = luaL_checknumber(l, 1);
    LuaTimer* luaTimer = (LuaTimer*)lua_newuserdata(l, sizeof(LuaTimer));
    luaTimer->timer = CreateTimer(minDelay, luaTimer, OnLuaTimerTriggered);
    luaTimer->listener = GetLuaEventListener(l, "TimerTriggered");
    return 1;
}

static int Lua_DestroyTimer( lua_State* l )
{
    LuaTimer* luaTimer = (LuaTimer*)lua_touserdata(l, 1);
    DestroyTimer(luaTimer->timer);
    return 0;
}

void RegisterTimeInLua()
{
    RegisterFunctionInLua("GetTime", Lua_GetTime);
    RegisterFunctionInLua("CreateTimer", Lua_CreateTimer);
    RegisterFunctionInLua("DestroyTimer", Lua_DestroyTimer);
}
