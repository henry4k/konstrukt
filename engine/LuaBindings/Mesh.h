#ifndef __APOAPSIS_LUA_BINDINGS_MESH__
#define __APOAPSIS_LUA_BINDINGS_MESH__

#include "../Mesh.h"
#include "../Lua.h"

bool RegisterMeshInLua();

Mesh* GetMeshFromLua( lua_State* l, int stackPosition );
Mesh* CheckMeshFromLua( lua_State* l, int stackPosition );

#endif
