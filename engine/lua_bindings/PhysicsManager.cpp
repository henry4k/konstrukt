#include <string.h> // memset

#include "../Lua.h"
#include "../PhysicsManager.h"
#include "Math.h"
#include "PhysicsManager.h"


// --- Collision Shape ---

static int CreateLuaCollisionShape( lua_State* l, CollisionShape* shape )
{
    if(shape)
    {
        PushPointerToLua(l, shape);
        ReferenceCollisionShape(shape);
        return 1;
    }
    else
    {
        luaL_error(l, "Can't create collision shape.");
        return 0;
    }
}

static int Lua_DestroyCollisionShape( lua_State* l )
{
    CollisionShape* shape = CheckCollisionShapeFromLua(l, 1);
    ReleaseCollisionShape(shape);
    return 0;
}

static int Lua_CreateEmptyCollisionShape( lua_State* l )
{
    return CreateLuaCollisionShape(l, CreateEmptyCollisionShape());
}

static int Lua_CreateBoxCollisionShape( lua_State* l )
{
    const glm::vec3 halfWidth(luaL_checknumber(l, 1),
                              luaL_checknumber(l, 2),
                              luaL_checknumber(l, 3));
    return CreateLuaCollisionShape(l, CreateBoxCollisionShape(halfWidth));
}

static int Lua_CreateSphereCollisionShape( lua_State* l )
{
    const float radius = luaL_checknumber(l, 1);
    return CreateLuaCollisionShape(l, CreateSphereCollisionShape(radius));
}

static int Lua_CreateCapsuleCollisionShape( lua_State* l )
{
    const float radius = luaL_checknumber(l, 1);
    const float height = luaL_checknumber(l, 2);
    return CreateLuaCollisionShape(l, CreateCapsuleCollisionShape(radius, height));
}

static int Lua_CreateCompoundCollisionShape( lua_State* l )
{
    // TODO
    //CollisionShape* shape = CreateCompoundCollisionShape(...);
    return CreateLuaCollisionShape(l, NULL);
}

CollisionShape* GetCollisionShapeFromLua( lua_State* l, int stackPosition )
{
    return (CollisionShape*)GetPointerFromLua(l, stackPosition);
}

CollisionShape* CheckCollisionShapeFromLua( lua_State* l, int stackPosition )
{
    return (CollisionShape*)CheckPointerFromLua(l, stackPosition);
}


// --- Force ---

static int Lua_CreateForce( lua_State* l )
{
    Solid* solid = CheckSolidFromLua(l, 1);
    Force* force = CreateForce(solid);
    if(force)
    {
        PushPointerToLua(l, force);
        return 1;
    }
    else
    {
        luaL_error(l, "Can't create force.");
        return 0;
    }
}

static int Lua_DestroyForce( lua_State* l )
{
    Force* force = CheckForceFromLua(l, 1);
    DestroyForce(force);
    return 0;
}

static int Lua_SetForce( lua_State* l )
{
    Force* force = CheckForceFromLua(l, 1);
    const glm::vec3 value(luaL_checknumber(l, 2),
                          luaL_checknumber(l, 3),
                          luaL_checknumber(l, 4));
    const glm::vec3 relativePosition(luaL_checknumber(l, 5),
                                     luaL_checknumber(l, 6),
                                     luaL_checknumber(l, 7));
    const bool useLocalCoordinates = (bool)lua_toboolean(l, 8);
    SetForce(force, value, relativePosition, useLocalCoordinates);
    return 0;
}

Force* GetForceFromLua( lua_State* l, int stackPosition )
{
    return (Force*)GetPointerFromLua(l, stackPosition);
}

Force* CheckForceFromLua( lua_State* l, int stackPosition )
{
    return (Force*)CheckPointerFromLua(l, stackPosition);
}


// --- Solid ---

static int Lua_CreateSolid( lua_State* l )
{
    const float mass = luaL_checknumber(l, 1);
    const glm::vec3 position(luaL_checknumber(l, 2),
                             luaL_checknumber(l, 3),
                             luaL_checknumber(l, 4));
    const glm::quat rotation = *CheckQuaternionFromLua(l, 5);
    CollisionShape* shape = CheckCollisionShapeFromLua(l, 6);

    Solid* solid = CreateSolid(mass, position, rotation, shape);
    if(solid)
    {
        PushPointerToLua(l, solid);
        ReferenceSolid(solid);
        return 1;
    }
    else
    {
        luaL_error(l, "Can't create solid.");
        return 0;
    }
}

static int Lua_DestroySolid( lua_State* l )
{
    Solid* solid = CheckSolidFromLua(l, 1);
    ReleaseSolid(solid);
    return 0;
}

static int Lua_GetSolidMass( lua_State* l )
{
    const Solid* solid = CheckSolidFromLua(l, 1);
    const float mass = GetSolidMass(solid);
    lua_pushnumber(l, mass);
    return 1;
}

static int Lua_SetSolidMass( lua_State* l )
{
    const Solid* solid = CheckSolidFromLua(l, 1);
    const float mass = luaL_checknumber(l, 2);
    SetSolidMass(solid, mass);
    return 0;
}

static int Lua_SetSolidRestitution( lua_State* l )
{
    const Solid* solid = CheckSolidFromLua(l, 1);
    const float restitution = luaL_checknumber(l, 2);
    SetSolidRestitution(solid, restitution);
    return 0;
}

static int Lua_SetSolidFriction( lua_State* l )
{
    const Solid* solid = CheckSolidFromLua(l, 1);
    const float friction = luaL_checknumber(l, 2);
    SetSolidFriction(solid, friction);
    return 0;
}

static int Lua_SetSolidCollisionThreshold( lua_State* l )
{
    Solid* solid = CheckSolidFromLua(l, 1);
    const float threshold = luaL_checknumber(l, 2);
    SetSolidCollisionThreshold(solid, threshold);
    return 0;
}

static int Lua_GetSolidPosition( lua_State* l )
{
    const Solid* solid = CheckSolidFromLua(l, 1);
    const glm::vec3 position = GetSolidPosition(solid);
    lua_pushnumber(l, position[0]);
    lua_pushnumber(l, position[1]);
    lua_pushnumber(l, position[2]);
    return 3;
}

static int Lua_GetSolidRotation( lua_State* l )
{
    const Solid* solid = CheckSolidFromLua(l, 1);
    glm::quat* rotation = CreateQuaternionInLua(l);
    *rotation = GetSolidRotation(solid);
    return 1;
}

static int Lua_GetSolidLinearVelocity( lua_State* l )
{
    const Solid* solid = CheckSolidFromLua(l, 1);
    const glm::vec3 linearVelocity = GetSolidLinearVelocity(solid);
    lua_pushnumber(l, linearVelocity[0]);
    lua_pushnumber(l, linearVelocity[1]);
    lua_pushnumber(l, linearVelocity[2]);
    return 3;
}

static int Lua_GetSolidAngularVelocity( lua_State* l )
{
    const Solid* solid = CheckSolidFromLua(l, 1);
    const glm::vec3 angularVelocity = GetSolidAngularVelocity(solid);
    lua_pushnumber(l, angularVelocity[0]);
    lua_pushnumber(l, angularVelocity[1]);
    lua_pushnumber(l, angularVelocity[2]);
    return 3;
}

static int Lua_ApplySolidImpulse( lua_State* l )
{
    const Solid* solid = CheckSolidFromLua(l, 1);
    const glm::vec3 impulse(luaL_checknumber(l, 2),
                            luaL_checknumber(l, 3),
                            luaL_checknumber(l, 4));
    const glm::vec3 relativePosition(luaL_checknumber(l, 5),
                                     luaL_checknumber(l, 6),
                                     luaL_checknumber(l, 7));
    const bool useLocalCoordinates = (bool)lua_toboolean(l, 8);
    ApplySolidImpulse(solid, impulse, relativePosition, useLocalCoordinates);
    return 0;
}

Solid* GetSolidFromLua( lua_State* l, int stackPosition )
{
    return (Solid*)GetPointerFromLua(l, stackPosition);
}

Solid* CheckSolidFromLua( lua_State* l, int stackPosition )
{
    return (Solid*)CheckPointerFromLua(l, stackPosition);
}


// --- Collision ---

const char* COLLISION_EVENT_NAME = "Collision";

static int CollisionEvent = INVALID_LUA_EVENT;

static void LuaCollisionCallback( const Collision* collision )
{
    lua_State* l = GetLuaState();

    PushPointerToLua(l, collision->a); // 1
    PushPointerToLua(l, collision->b); // 2
    lua_pushnumber(l, collision->pointOnA[0]); // 3
    lua_pushnumber(l, collision->pointOnA[1]);
    lua_pushnumber(l, collision->pointOnA[2]);
    lua_pushnumber(l, collision->pointOnB[0]); // 6
    lua_pushnumber(l, collision->pointOnB[1]);
    lua_pushnumber(l, collision->pointOnB[2]);
    lua_pushnumber(l, collision->normalOnB[0]); // 9
    lua_pushnumber(l, collision->normalOnB[1]);
    lua_pushnumber(l, collision->normalOnB[2]);
    lua_pushnumber(l, collision->impulse); // 12

    FireLuaEvent(l, CollisionEvent, 12, false);
}


// --- Register in Lua ---

bool RegisterPhysicsManagerInLua()
{
    CollisionEvent = RegisterLuaEvent(COLLISION_EVENT_NAME);
    if(CollisionEvent == INVALID_LUA_EVENT)
        return false;

    SetCollisionCallback(LuaCollisionCallback);

    return
        RegisterFunctionInLua("DestroyCollisionShape", Lua_DestroyCollisionShape) &&
        RegisterFunctionInLua("CreateEmptyCollisionShape", Lua_CreateEmptyCollisionShape) &&
        RegisterFunctionInLua("CreateBoxCollisionShape", Lua_CreateBoxCollisionShape) &&
        RegisterFunctionInLua("CreateSphereCollisionShape", Lua_CreateSphereCollisionShape) &&
        RegisterFunctionInLua("CreateCapsuleCollisionShape", Lua_CreateCapsuleCollisionShape) &&
        RegisterFunctionInLua("CreateCompoundCollisionShape", Lua_CreateCompoundCollisionShape) &&

        RegisterFunctionInLua("CreateForce", Lua_CreateForce) &&
        RegisterFunctionInLua("DestroyForce", Lua_DestroyForce) &&
        RegisterFunctionInLua("SetForce", Lua_SetForce) &&

        RegisterFunctionInLua("CreateSolid", Lua_CreateSolid) &&
        RegisterFunctionInLua("DestroySolid", Lua_DestroySolid) &&
        RegisterFunctionInLua("GetSolidMass", Lua_GetSolidMass) &&
        RegisterFunctionInLua("SetSolidMass", Lua_SetSolidMass) &&
        RegisterFunctionInLua("SetSolidRestitution", Lua_SetSolidRestitution) &&
        RegisterFunctionInLua("SetSolidFriction", Lua_SetSolidFriction) &&
        RegisterFunctionInLua("SetSolidCollisionThreshold", Lua_SetSolidCollisionThreshold) &&
        RegisterFunctionInLua("GetSolidPosition", Lua_GetSolidPosition) &&
        RegisterFunctionInLua("GetSolidRotation", Lua_GetSolidRotation) &&
        RegisterFunctionInLua("GetSolidLinearVelocity", Lua_GetSolidLinearVelocity) &&
        RegisterFunctionInLua("GetSolidAngularVelocity", Lua_GetSolidAngularVelocity) &&
        RegisterFunctionInLua("ApplySolidImpulse", Lua_ApplySolidImpulse);
}
