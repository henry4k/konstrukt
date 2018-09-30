#ifndef __KONSTRUKT_LUA_BINDINGS_LUA_BUFFER__
#define __KONSTRUKT_LUA_BINDINGS_LUA_BUFFER__

struct lua_State;
struct LuaBuffer;

LuaBuffer* GetLuaBufferFromLua( lua_State* l, int stackPosition );
LuaBuffer* CheckLuaBufferFromLua( lua_State* l, int stackPosition );

void RegisterLuaBufferInLua();

#endif
