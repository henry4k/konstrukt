#ifndef __KONSTRUKT_LUA_BINDINGS_TIME__
#define __KONSTRUKT_LUA_BINDINGS_TIME__

struct lua_State;
struct Timer;

Timer* GetTimerFromLua( lua_State* l, int stackPosition );
Timer* CheckTimerFromLua( lua_State* l, int stackPosition );

void RegisterTimeInLua();

#endif
