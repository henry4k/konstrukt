#ifndef __KONSTRUKT_LUA_BINDINGS_TEXTURE__
#define __KONSTRUKT_LUA_BINDINGS_TEXTURE__

struct lua_State;
struct Texture;

Texture* GetTextureFromLua( lua_State* l, int stackPosition );
Texture* CheckTextureFromLua( lua_State* l, int stackPosition );

void RegisterTextureInLua();

#endif
