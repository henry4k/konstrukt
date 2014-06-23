#ifndef __APOAPSIS_LUA_BINDINGS_TEXTURE__
#define __APOAPSIS_LUA_BINDINGS_TEXTURE__

#include "../Texture.h"
#include "../Lua.h"

bool RegisterTextureInLua();

Texture* GetTextureFromLua( lua_State* l, int stackPosition );
Texture* CheckTextureFromLua( lua_State* l, int stackPosition );

#endif
