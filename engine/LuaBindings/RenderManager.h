#ifndef __APOAPSIS_LUA_BINDINGS_RENDER_MANAGER__
#define __APOAPSIS_LUA_BINDINGS_RENDER_MANAGER__

#include "../RenderManager.h"
#include "../Lua.h"

GraphicsObject* GetGraphicsObjectFromLua( lua_State* l, int stackPosition );
GraphicsObject* CheckGraphicsObjectFromLua( lua_State* l, int stackPosition );

#endif
