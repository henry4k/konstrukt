#ifndef __APOPASIS_LUA_BINDINGS_MESH_BUFFER__
#define __APOPASIS_LUA_BINDINGS_MESH_BUFFER__

#include "../MeshBuffer.h"
#include "../Lua.h"

bool RegisterMeshBufferInLua();

MeshBuffer* GetMeshBufferFromLua( lua_State* l, int stackPosition );
MeshBuffer* CheckMeshBufferFromLua( lua_State* l, int stackPosition );

#endif
