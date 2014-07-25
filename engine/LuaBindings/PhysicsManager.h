#ifndef __APOAPSIS_LUA_BINDINGS_PHYSICS_MANAGER__
#define __APOAPSIS_LUA_BINDINGS_PHYSICS_MANAGER__

#include "../PhysicsManager.h"
#include "../Lua.h"

bool RegisterPhysicsManagerInLua();

CollisionShape* GetCollisionShapeFromLua( lua_State* l, int stackPosition );
CollisionShape* CheckCollisionShapeFromLua( lua_State* l, int stackPosition );
Solid* GetSolidFromLua( lua_State* l, int stackPosition );
Solid* CheckSolidFromLua( lua_State* l, int stackPosition );

#endif
