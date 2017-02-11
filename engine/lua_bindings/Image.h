#ifndef __KONSTRUKT_LUA_BINDINGS_IMAGE__
#define __KONSTRUKT_LUA_BINDINGS_IMAGE__

struct lua_State;
struct Image;

Image* GetImageFromLua( lua_State* l, int stackPosition );
Image* CheckImageFromLua( lua_State* l, int stackPosition );

void RegisterImageInLua();

#endif
