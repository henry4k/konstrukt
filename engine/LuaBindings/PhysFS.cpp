#include "../Lua.h"
#include "../PhysFS.h"


int Lua_MountPackage( lua_State* l )
{
    const char* name = luaL_checkstring(l, 1);
    const bool result = MountPackage(name);
    lua_pushboolean(l, result);
    return 1;
}

int Lua_UnmountPackage( lua_State* l )
{
    const char* name = luaL_checkstring(l, 1);
    UnmountPackage(name);
    return 0;
}

bool RegisterPhysFSInLua()
{
    return
        RegisterFunctionInLua("MountPackage", Lua_MountPackage) &&
        RegisterFunctionInLua("UnmountPackage", Lua_UnmountPackage);
}
