#ifndef __APOAPSIS_LUA_BINDINGS_TEXTURE__
#define __APOAPSIS_LUA_BINDINGS_TEXTURE__

struct lua_State;
struct Texture;

Texture* GetTextureFromLua( lua_State* l, int stackPosition );
Texture* CheckTextureFromLua( lua_State* l, int stackPosition );

bool RegisterTextureInLua();

#endif
