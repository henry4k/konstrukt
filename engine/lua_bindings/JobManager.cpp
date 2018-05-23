#include "../Lua.h"
#include "JobManager.h"


void PushJobToLua( lua_State* l, JobId job )
{
    lua_pushinteger(l, job);
}

JobId GetJobFromLua( lua_State* l, int stackPosition )
{
    return (JobId)lua_tointeger(l, stackPosition);
}

JobId CheckJobFromLua( lua_State* l, int stackPosition )
{
    return luaL_checkinteger(l, stackPosition);
}

static int Lua_JobIsComplete( lua_State* l )
{
    const JobId job = CheckJobFromLua(l, 1);
    lua_pushboolean(l, GetJobStatus(job) == COMPLETED_JOB);
    return 1;
}

void RegisterJobManagerInLua()
{
    RegisterFunctionInLua("JobIsComplete", Lua_JobIsComplete);
}
