#ifndef __APOAPSIS_LUA_BINDINGS_MODEL_MANAGER__
#define __APOAPSIS_LUA_BINDINGS_MODEL_MANAGER__

#include "../ModelManager.h"
#include "../Lua.h"

bool RegisterModelManagerInLua();

Model* GetModelFromLua( lua_State* l, int stackPosition );
Model* CheckModelFromLua( lua_State* l, int stackPosition );

#endif
