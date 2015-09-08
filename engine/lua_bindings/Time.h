#ifndef __APOAPSIS_LUA_BINDINGS_TIME__
#define __APOAPSIS_LUA_BINDINGS_TIME__

struct lua_State;
struct Timer;

Timer* GetTimerFromLua( lua_State* l, int stackPosition );
Timer* CheckTimerFromLua( lua_State* l, int stackPosition );

bool RegisterTimeInLua();

#endif
