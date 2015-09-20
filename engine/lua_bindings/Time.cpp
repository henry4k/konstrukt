#include "../Lua.h"
#include "../Time.h"
#include "Time.h"


const char* TIMER_TRIGGERED_EVENT_NAME = "TimerTriggered";

int g_TimerTriggeredEvent = INVALID_LUA_EVENT;


static int Lua_GetTime( lua_State* l )
{
    lua_pushnumber(l, GetTime());
    return 1;
}

static void OnLuaTimerTriggered( Timer* timer, double timeDelta, void* context )
{
    lua_State* l = GetLuaState();
    PushPointerToLua(l, timer);
    lua_pushnumber(l, timeDelta);
    FireLuaEvent(l, g_TimerTriggeredEvent, 2, false);
}

static int Lua_CreateTimer( lua_State* l )
{
    const double minDelay = luaL_checknumber(l, 1);
    Timer* timer = CreateTimer(minDelay, NULL, OnLuaTimerTriggered);
    PushPointerToLua(l, timer);
    return 1;
}

static int Lua_DestroyTimer( lua_State* l )
{
    Timer* timer = CheckTimerFromLua(l, 1);
    DestroyTimer(timer);
    return 0;
}

Timer* GetTimerFromLua( lua_State* l, int stackPosition )
{
    return (Timer*)GetPointerFromLua(l, stackPosition);
}

Timer* CheckTimerFromLua( lua_State* l, int stackPosition )
{
    return (Timer*)CheckPointerFromLua(l, stackPosition);
}

bool RegisterTimeInLua()
{
    g_TimerTriggeredEvent = RegisterLuaEvent(TIMER_TRIGGERED_EVENT_NAME);
    if(g_TimerTriggeredEvent == INVALID_LUA_EVENT)
        return false;

    return
        RegisterFunctionInLua("GetTime", Lua_GetTime) &&
        RegisterFunctionInLua("CreateTimer", Lua_CreateTimer) &&
        RegisterFunctionInLua("DestroyTimer", Lua_DestroyTimer);
}
