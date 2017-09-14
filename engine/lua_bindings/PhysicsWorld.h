#ifndef __KONSTRUKT_LUA_BINDINGS_PHYSICS_WORLD__
#define __KONSTRUKT_LUA_BINDINGS_PHYSICS_WORLD__

#include "../PhysicsWorld.h"

struct lua_State;

PhysicsWorld* GetPhysicsWorldFromLua( lua_State* l, int stackPosition );
PhysicsWorld* CheckPhysicsWorldFromLua( lua_State* l, int stackPosition );

CollisionShape* GetCollisionShapeFromLua( lua_State* l, int stackPosition );
CollisionShape* CheckCollisionShapeFromLua( lua_State* l, int stackPosition );

ForceId GetForceFromLua( lua_State* l, int stackPosition );
ForceId CheckForceFromLua( lua_State* l, int stackPosition );

SolidProperties* GetSolidPropertiesFromLua( lua_State* l, int stackPosition );
SolidProperties* CheckSolidPropertiesFromLua( lua_State* l, int stackPosition );

SolidMotionState* GetSolidMotionStateFromLua( lua_State* l, int stackPosition );
SolidMotionState* CheckSolidMotionStateFromLua( lua_State* l, int stackPosition );

SolidId GetSolidFromLua( lua_State* l, int stackPosition );
SolidId CheckSolidFromLua( lua_State* l, int stackPosition );

void RegisterPhysicsWorldInLua();

#endif
