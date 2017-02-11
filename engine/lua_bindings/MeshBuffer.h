#ifndef __KONSTRUKT_LUA_BINDINGS_MESH_BUFFER__
#define __KONSTRUKT_LUA_BINDINGS_MESH_BUFFER__

struct lua_State;
struct MeshBuffer;

MeshBuffer* GetMeshBufferFromLua( lua_State* l, int stackPosition );
MeshBuffer* CheckMeshBufferFromLua( lua_State* l, int stackPosition );

void RegisterMeshBufferInLua();

#endif
