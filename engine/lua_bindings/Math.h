#ifndef __KONSTRUKT_LUA_BINDINGS_MATH__
#define __KONSTRUKT_LUA_BINDINGS_MATH__

#include "../Math.h"

struct lua_State;

Mat4* CreateMatrix4InLua( lua_State* l );
Mat4* GetMatrix4FromLua( lua_State* l, int stackPosition );
Mat4* CheckMatrix4FromLua( lua_State* l, int stackPosition );

Quat* CreateQuaternionInLua( lua_State* l );
Quat* GetQuaternionFromLua( lua_State* l, int stackPosition );
Quat* CheckQuaternionFromLua( lua_State* l, int stackPosition );

int CheckTransformationFlagsFromLua( lua_State* l, int stackPosition );

void RegisterMathInLua();

#endif
