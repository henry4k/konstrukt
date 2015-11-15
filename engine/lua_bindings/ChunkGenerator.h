#ifndef __APOAPSIS_LUA_BINDINGS_CHUNK_GENERATOR__
#define __APOAPSIS_LUA_BINDINGS_CHUNK_GENERATOR__

struct lua_State;
struct ChunkGenerator;

ChunkGenerator* GetChunkGeneratorFromLua( lua_State* l, int stackPosition );
ChunkGenerator* CheckChunkGeneratorFromLua( lua_State* l, int stackPosition );

bool RegisterChunkGeneratorInLua();

#endif
