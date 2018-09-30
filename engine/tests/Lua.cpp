#include <string.h> // memset
#include "../JobManager.h"
#include "../Lua.h"
#include "TestTools.h"


static int Lua_AddOne( lua_State* l )
{
    int i = luaL_checkinteger(l, 1);
    lua_pushinteger(l, i+1);
    return 1;
}

InlineTest("can be initialized and destroyed")
{
    InitJobManager({1});
    InitLua();

    SetLuaWorkerCount(1);

    DestroyLua();
    DestroyJobManager();
}

InlineTest("can register functions")
{
    InitJobManager({1});
    InitLua();

    REGISTER_LUA_FUNCTION(AddOne);

    SetLuaWorkerCount(1);

    DestroyLua();
    DestroyJobManager();
}

int main( int argc, char** argv )
{
    InitTests(argc, argv);
    return RunTests();
}
