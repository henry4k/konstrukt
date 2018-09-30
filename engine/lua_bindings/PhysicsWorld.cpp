#include <string.h> // memset

#include "../Lua.h"
#include "../PhysicsWorld.h"
#include "Math.h"
#include "PhysicsWorld.h"


// To suppress this C++11 warning:
// narrowing conversion of ‘luaL_checknumber(l, 2)’ from
// ‘lua_Number {aka double}’ to ‘float’ inside { } is ill-formed in C++11
static Vec3 CreateVec3( float x, float y, float z )
{
    Vec3 r = {{x,y,z}};
    return r;
}


// --- PhysicsWorld ---

static int Lua_CreatePhysicsWorld( lua_State* l )
{
    PushPointerToLua(l, CreatePhysicsWorld());
    return 1;
}

static int Lua_DestroyPhysicsWorld( lua_State* l )
{
    DestroyPhysicsWorld(CheckPhysicsWorldFromLua(l, 1));
    return 0;
}

static int Lua_SetGravity( lua_State* l )
{
    PhysicsWorld* world = CheckPhysicsWorldFromLua(l, 1);
    const Vec3 force = CreateVec3(luaL_checknumber(l, 2),
                                  luaL_checknumber(l, 3),
                                  luaL_checknumber(l, 4));
    SetGravity(world, force);
    return 0;
}

PhysicsWorld* GetPhysicsWorldFromLua( lua_State* l, int stackPosition )
{
    return (PhysicsWorld*)GetPointerFromLua(l, stackPosition);
}

PhysicsWorld* CheckPhysicsWorldFromLua( lua_State* l, int stackPosition )
{
    return (PhysicsWorld*)CheckPointerFromLua(l, stackPosition);
}


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
    const Vec3 halfWidth = CreateVec3(luaL_checknumber(l, 1),
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


// --- Collision ---

const char* COLLISION_EVENT_NAME = "Collision";

static int CollisionEvent = INVALID_LUA_EVENT;

static void LuaCollisionCallback( PhysicsWorld* world, const Collision* collision )
{
    // TODO: Implement via new event system
    //lua_State* l = GetLuaState();

    //PushIdToLua(l, collision->a); // 1
    //PushIdToLua(l, collision->b); // 2
    //lua_pushnumber(l, collision->pointOnA._[0]); // 3
    //lua_pushnumber(l, collision->pointOnA._[1]);
    //lua_pushnumber(l, collision->pointOnA._[2]);
    //lua_pushnumber(l, collision->pointOnB._[0]); // 6
    //lua_pushnumber(l, collision->pointOnB._[1]);
    //lua_pushnumber(l, collision->pointOnB._[2]);
    //lua_pushnumber(l, collision->normalOnB._[0]); // 9
    //lua_pushnumber(l, collision->normalOnB._[1]);
    //lua_pushnumber(l, collision->normalOnB._[2]);
    //lua_pushnumber(l, collision->impulse); // 12

    //FireLuaEvent(l, CollisionEvent, 12, false);
}


// --- SolidProperties ---

static const char* SOLID_PROPERTIES_TYPE = "SolidProperties";

static int Lua_PackSolidProperties( lua_State* l )
{
    SolidProperties* properties =
        (SolidProperties*)PushUserDataToLua(l,
                                            SOLID_PROPERTIES_TYPE,
                                            sizeof(SolidProperties));
    properties->mass               = luaL_checknumber(l, 1);
    properties->restitution        = luaL_checknumber(l, 2);
    properties->friction           = luaL_checknumber(l, 3);
    properties->collisionThreshold = luaL_checknumber(l, 4);
    properties->affectedByGravity  = lua_toboolean(l, 5);
    return 1;
}

static int Lua_UnpackSolidProperties( lua_State* l )
{
    const SolidProperties* properties = CheckSolidPropertiesFromLua(l, 1);
    lua_pushnumber(l, properties->mass);
    lua_pushnumber(l, properties->restitution);
    lua_pushnumber(l, properties->friction);
    lua_pushnumber(l, properties->collisionThreshold);
    lua_pushboolean(l, properties->affectedByGravity);
    return 1;
}

SolidProperties* GetSolidPropertiesFromLua( lua_State* l, int stackPosition )
{
    return (SolidProperties*)GetPointerFromLua(l, stackPosition);
}

SolidProperties* CheckSolidPropertiesFromLua( lua_State* l, int stackPosition )
{
    return (SolidProperties*)CheckPointerFromLua(l, stackPosition);
}


// --- SolidMotionState ---

static const char* SOLID_MOTION_STATE_TYPE = "SolidMotionState";

static int Lua_PackSolidMotionState( lua_State* l )
{
    SolidMotionState* state =
        (SolidMotionState*)PushUserDataToLua(l,
                                            SOLID_MOTION_STATE_TYPE,
                                            sizeof(SolidMotionState));
    state->position._[0] = luaL_checknumber(l, 1);
    state->position._[1] = luaL_checknumber(l, 2);
    state->position._[2] = luaL_checknumber(l, 3);

    state->rotation = *CheckQuaternionFromLua(l, 4);

    state->linearVelocity._[0] = luaL_checknumber(l, 5);
    state->linearVelocity._[1] = luaL_checknumber(l, 6);
    state->linearVelocity._[2] = luaL_checknumber(l, 7);

    state->angularVelocity._[0] = luaL_checknumber(l, 8);
    state->angularVelocity._[1] = luaL_checknumber(l, 9);
    state->angularVelocity._[2] = luaL_checknumber(l, 10);
    return 1;
}

static int Lua_UnpackSolidMotionState( lua_State* l )
{
    const SolidMotionState* state = CheckSolidMotionStateFromLua(l, 1);

    lua_pushnumber(l, state->position._[0]);
    lua_pushnumber(l, state->position._[1]);
    lua_pushnumber(l, state->position._[2]);

    *CreateQuaternionInLua(l) = state->rotation;

    lua_pushnumber(l, state->linearVelocity._[0]);
    lua_pushnumber(l, state->linearVelocity._[1]);
    lua_pushnumber(l, state->linearVelocity._[2]);

    lua_pushnumber(l, state->angularVelocity._[0]);
    lua_pushnumber(l, state->angularVelocity._[1]);
    lua_pushnumber(l, state->angularVelocity._[2]);

    return 1;
}

SolidMotionState* GetSolidMotionStateFromLua( lua_State* l, int stackPosition )
{
    return (SolidMotionState*)GetPointerFromLua(l, stackPosition);
}

SolidMotionState* CheckSolidMotionStateFromLua( lua_State* l, int stackPosition )
{
    return (SolidMotionState*)CheckPointerFromLua(l, stackPosition);
}


// --- Solid ---

static int Lua_CreateSolid( lua_State* l )
{
    PhysicsWorld* world = CheckPhysicsWorldFromLua(l, 1);
    const SolidProperties* properties = CheckSolidPropertiesFromLua(l, 2);
    const SolidMotionState* motionState = CheckSolidMotionStateFromLua(l, 3);
    CollisionShape* shape = CheckCollisionShapeFromLua(l, 4);

    SolidId solid = CreateSolid(world, properties, motionState, shape);
    ReferenceSolid(world, solid);
    PushIdToLua(l, solid);
    return 1;
}

static int Lua_DestroySolid( lua_State* l )
{
    PhysicsWorld* world = CheckPhysicsWorldFromLua(l, 1);
    SolidId solid = CheckSolidFromLua(l, 2);
    ReleaseSolid(world, solid);
    return 0;
}

static int Lua_SetSolidProperties( lua_State* l )
{
    PhysicsWorld* world = CheckPhysicsWorldFromLua(l, 1);
    const SolidId solid = CheckSolidFromLua(l, 2);
    const SolidProperties* properties = CheckSolidPropertiesFromLua(l, 3);
    SetSolidProperties(world, solid, properties);
    return 0;
}

static int Lua_GetSolidMotionState( lua_State* l )
{
    PhysicsWorld* world = CheckPhysicsWorldFromLua(l, 1);
    const SolidId solid = CheckSolidFromLua(l, 2);
    const SolidMotionState* state = GetSolidMotionState(world, solid);
    CopyUserDataToLua(l, SOLID_MOTION_STATE_TYPE, sizeof(SolidMotionState), &state);
    return 1;
}

static int Lua_ApplySolidImpulse( lua_State* l )
{
    PhysicsWorld* world = CheckPhysicsWorldFromLua(l, 1);
    const SolidId solid = CheckSolidFromLua(l, 2);
    const Vec3 impulse = CreateVec3(luaL_checknumber(l, 3),
                                    luaL_checknumber(l, 4),
                                    luaL_checknumber(l, 5));
    const Vec3 relativePosition = CreateVec3(luaL_checknumber(l, 6),
                                             luaL_checknumber(l, 7),
                                             luaL_checknumber(l, 8));
    const bool useLocalCoordinates = (bool)lua_toboolean(l, 9);
    ApplySolidImpulse(world, solid, impulse, relativePosition, useLocalCoordinates);
    return 0;
}

SolidId GetSolidFromLua( lua_State* l, int stackPosition )
{
    return (SolidId)GetIdFromLua(l, stackPosition);
}

SolidId CheckSolidFromLua( lua_State* l, int stackPosition )
{
    return (SolidId)CheckIdFromLua(l, stackPosition);
}


// --- Force ---

static int Lua_CreateForce( lua_State* l )
{
    PhysicsWorld* world = CheckPhysicsWorldFromLua(l, 1);
    SolidId solid = CheckSolidFromLua(l, 2);
    PushIdToLua(l, CreateForce(world, solid));
    return 1;
}

static int Lua_DestroyForce( lua_State* l )
{
    PhysicsWorld* world = CheckPhysicsWorldFromLua(l, 1);
    ForceId force = CheckForceFromLua(l, 2);
    DestroyForce(world, force);
    return 0;
}

static int Lua_SetForce( lua_State* l )
{
    PhysicsWorld* world = CheckPhysicsWorldFromLua(l, 1);
    ForceId force = CheckForceFromLua(l, 2);
    const Vec3 value = CreateVec3(luaL_checknumber(l, 3),
                                  luaL_checknumber(l, 4),
                                  luaL_checknumber(l, 5));
    const Vec3 relativePosition = CreateVec3(luaL_checknumber(l, 6),
                                             luaL_checknumber(l, 7),
                                             luaL_checknumber(l, 8));
    const bool useLocalCoordinates = (bool)lua_toboolean(l, 9);
    SetForce(world, force, value, relativePosition, useLocalCoordinates);
    return 0;
}

ForceId GetForceFromLua( lua_State* l, int stackPosition )
{
    return (ForceId)GetIdFromLua(l, stackPosition);
}

ForceId CheckForceFromLua( lua_State* l, int stackPosition )
{
    return (ForceId)CheckIdFromLua(l, stackPosition);
}


// --- Register in Lua ---

void RegisterPhysicsWorldInLua()
{
    RegisterFunctionInLua("CreatePhysicsWorld", Lua_CreatePhysicsWorld);
    RegisterFunctionInLua("DestroyPhysicsWorld", Lua_DestroyPhysicsWorld);
    RegisterFunctionInLua("SetGravity", Lua_SetGravity);

    RegisterFunctionInLua("DestroyCollisionShape", Lua_DestroyCollisionShape);
    RegisterFunctionInLua("CreateEmptyCollisionShape", Lua_CreateEmptyCollisionShape);
    RegisterFunctionInLua("CreateBoxCollisionShape", Lua_CreateBoxCollisionShape);
    RegisterFunctionInLua("CreateSphereCollisionShape", Lua_CreateSphereCollisionShape);
    RegisterFunctionInLua("CreateCapsuleCollisionShape", Lua_CreateCapsuleCollisionShape);
    RegisterFunctionInLua("CreateCompoundCollisionShape", Lua_CreateCompoundCollisionShape);

    CollisionEvent = RegisterLuaEvent(COLLISION_EVENT_NAME);
    SetCollisionCallback(LuaCollisionCallback);

    RegisterUserDataTypeInLua(SOLID_PROPERTIES_TYPE, NULL);
    RegisterFunctionInLua("PackSolidProperties", Lua_PackSolidProperties);
    RegisterFunctionInLua("UnpackSolidProperties", Lua_UnpackSolidProperties);

    RegisterUserDataTypeInLua(SOLID_MOTION_STATE_TYPE, NULL);
    RegisterFunctionInLua("PackSolidMotionState", Lua_PackSolidMotionState);
    RegisterFunctionInLua("UnpackSolidMotionState", Lua_UnpackSolidMotionState);

    RegisterFunctionInLua("CreateSolid", Lua_CreateSolid);
    RegisterFunctionInLua("DestroySolid", Lua_DestroySolid);
    RegisterFunctionInLua("SetSolidProperties", Lua_SetSolidProperties);
    RegisterFunctionInLua("GetSolidMotionState", Lua_GetSolidMotionState);
    RegisterFunctionInLua("ApplySolidImpulse", Lua_ApplySolidImpulse);

    RegisterFunctionInLua("CreateForce", Lua_CreateForce);
    RegisterFunctionInLua("DestroyForce", Lua_DestroyForce);
    RegisterFunctionInLua("SetForce", Lua_SetForce);
}
