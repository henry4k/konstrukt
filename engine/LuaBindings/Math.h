#ifndef __APOAPSIS_LUA_BINDINGS_MATH__
#define __APOAPSIS_LUA_BINDINGS_MATH__

#include "../Math.h"

struct lua_State;

glm::mat4* CreateMatrix4InLua( lua_State* l );
glm::mat4* GetMatrix4FromLua( lua_State* l, int stackPosition );
glm::mat4* CheckMatrix4FromLua( lua_State* l, int stackPosition );

glm::quat* CreateQuaternionInLua( lua_State* l );
glm::quat* GetQuaternionFromLua( lua_State* l, int stackPosition );
glm::quat* CheckQuaternionFromLua( lua_State* l, int stackPosition );

bool RegisterMathInLua();

#endif
