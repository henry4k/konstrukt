#ifndef __KONSTRUKT_LUA_BINDINGS_JOB_MANAGER__
#define __KONSTRUKT_LUA_BINDINGS_JOB_MANAGER__

#include "../JobManager.h" // JobId

struct lua_State;

void PushJobToLua( lua_State* l, JobId job );
JobId GetJobFromLua( lua_State* l, int stackPosition );
JobId CheckJobFromLua( lua_State* l, int stackPosition );

void RegisterJobManagerInLua();

#endif
