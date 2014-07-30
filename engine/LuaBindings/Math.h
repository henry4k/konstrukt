#ifndef __APOAPSIS_LUA_BINDINGS_MATH__
#define __APOAPSIS_LUA_BINDINGS_MATH__

#include "../Lua.h"
#include "../Math.h"

bool RegisterMathInLua();

glm::mat4* GetMatrix4FromLua( lua_State* l, int stackPosition );
glm::mat4* CheckMatrix4FromLua( lua_State* l, int stackPosition );

glm::quat* GetQuaternionFromLua( lua_State* l, int stackPosition );
glm::quat* CheckQuaternionFromLua( lua_State* l, int stackPosition );

#endif
