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

int Lua_LoadFile( lua_State* l )
{
    const char* vfsPath = luaL_checkstring(l, 1);
    FileBuffer* buffer = LoadFile(vfsPath);
    lua_pushlstring(l, buffer->data, buffer->size);
    FreeFile(buffer);
    return 1;
}

bool RegisterPhysFSInLua()
{
    return
        RegisterFunctionInLua("MountPackage", Lua_MountPackage) &&
        RegisterFunctionInLua("UnmountPackage", Lua_UnmountPackage) &&
        RegisterFunctionInLua("LoadFile", Lua_LoadFile);
}
