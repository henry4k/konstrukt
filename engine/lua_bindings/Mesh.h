#ifndef __KONSTRUKT_LUA_BINDINGS_MESH__
#define __KONSTRUKT_LUA_BINDINGS_MESH__

struct lua_State;
struct Mesh;

Mesh* GetMeshFromLua( lua_State* l, int stackPosition );
Mesh* CheckMeshFromLua( lua_State* l, int stackPosition );

void RegisterMeshInLua();

#endif
