#ifndef __KONSTRUKT_LUA_BINDINGS_MESH_CHUNK_GENERATOR__
#define __KONSTRUKT_LUA_BINDINGS_MESH_CHUNK_GENERATOR__

struct lua_State;
struct MeshChunkGenerator;

MeshChunkGenerator* GetMeshChunkGeneratorFromLua( lua_State* l, int stackPosition );
MeshChunkGenerator* CheckMeshChunkGeneratorFromLua( lua_State* l, int stackPosition );

void RegisterMeshChunkGeneratorInLua();

#endif
