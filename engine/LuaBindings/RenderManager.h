#ifndef __APOAPSIS_LUA_BINDINGS_RENDER_MANAGER__
#define __APOAPSIS_LUA_BINDINGS_RENDER_MANAGER__

#include "../RenderManager.h"
#include "../Lua.h"

bool RegisterRenderManagerInLua();

Model* GetModelFromLua( lua_State* l, int stackPosition );
Model* CheckModelFromLua( lua_State* l, int stackPosition );

#endif
