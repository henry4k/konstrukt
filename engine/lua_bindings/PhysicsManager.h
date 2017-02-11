#ifndef __KONSTRUKT_LUA_BINDINGS_PHYSICS_MANAGER__
#define __KONSTRUKT_LUA_BINDINGS_PHYSICS_MANAGER__

struct lua_State;
struct CollisionShape;
struct Force;
struct Solid;

CollisionShape* GetCollisionShapeFromLua( lua_State* l, int stackPosition );
CollisionShape* CheckCollisionShapeFromLua( lua_State* l, int stackPosition );

Force* GetForceFromLua( lua_State* l, int stackPosition );
Force* CheckForceFromLua( lua_State* l, int stackPosition );

Solid* GetSolidFromLua( lua_State* l, int stackPosition );
Solid* CheckSolidFromLua( lua_State* l, int stackPosition );

void RegisterPhysicsManagerInLua();

#endif
