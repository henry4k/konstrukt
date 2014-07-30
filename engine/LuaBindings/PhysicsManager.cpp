#include <string.h> // memset

#include "../Lua.h"
#include "../PhysicsManager.h"
#include "PhysicsManager.h"


// --- Collision Shape ---

const char* COLLISION_SHAPE_TYPE = "CollisionShape";

int Lua_CollisionShape_destructor( lua_State* l )
{
    CollisionShape* shape =
        reinterpret_cast<CollisionShape*>(lua_touserdata(l, 1));
    ReleaseCollisionShape(shape);
    return 0;
}

static int CreateLuaCollisionShape( lua_State* l, CollisionShape* shape )
{
    if(shape &&
       CopyUserDataToLua(l, COLLISION_SHAPE_TYPE, sizeof(shape), &shape))
    {
        ReferenceCollisionShape(shape);
        return 1;
    }
    else
    {
        lua_pop(l, 1);
        luaL_error(l, "Can't create collision shape.");
        return 0;
    }
}

int Lua_CreateBoxCollisionShape( lua_State* l )
{
    const glm::vec3 halfWidth(luaL_checknumber(l, 1),
                              luaL_checknumber(l, 2),
                              luaL_checknumber(l, 3));
    return CreateLuaCollisionShape(l, CreateBoxCollisionShape(halfWidth));
}

int Lua_CreateSphereCollisionShape( lua_State* l )
{
    const float radius = luaL_checknumber(l, 1);
    return CreateLuaCollisionShape(l, CreateSphereCollisionShape(radius));
}

int Lua_CreateCompoundCollisionShape( lua_State* l )
{
    // TODO
    //CollisionShape* shape = CreateCompoundCollisionShape(...);
    return CreateLuaCollisionShape(l, NULL);
}

CollisionShape* GetCollisionShapeFromLua( lua_State* l, int stackPosition )
{
    return *(CollisionShape**)GetUserDataFromLua(l, stackPosition, COLLISION_SHAPE_TYPE);
}

CollisionShape* CheckCollisionShapeFromLua( lua_State* l, int stackPosition )
{
    return *(CollisionShape**)CheckUserDataFromLua(l, stackPosition, COLLISION_SHAPE_TYPE);
}


// --- Solid ---

const char* SOLID_TYPE = "Solid";

int Lua_Solid_destructor( lua_State* l )
{
    Solid* solid =
        reinterpret_cast<Solid*>(lua_touserdata(l, 1));
    ReleaseSolid(solid);
    return 0;
}

int Lua_CreateSolid( lua_State* l )
{
    const float mass = luaL_checknumber(l, 1);
    const glm::vec3 position(luaL_checknumber(l, 2),
                             luaL_checknumber(l, 3),
                             luaL_checknumber(l, 4));
    const glm::quat rotation(luaL_checknumber(l, 5),
                             luaL_checknumber(l, 6),
                             luaL_checknumber(l, 7),
                             luaL_checknumber(l, 8));
    CollisionShape* shape = CheckCollisionShapeFromLua(l, 9);

    Solid* solid = CreateSolid(mass, position, rotation, shape);
    if(solid &&
       CopyUserDataToLua(l, SOLID_TYPE, sizeof(solid), &solid))
    {
        ReferenceSolid(solid);
        return 1;
    }
    else
    {
        lua_pop(l, 1);
        luaL_error(l, "Can't create solid.");
        return 0;
    }
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
    const glm::quat rotation = GetSolidRotation(solid);
    lua_pushnumber(l, rotation[0]);
    lua_pushnumber(l, rotation[1]);
    lua_pushnumber(l, rotation[2]);
    lua_pushnumber(l, rotation[3]);
    return 4;
}

Solid* GetSolidFromLua( lua_State* l, int stackPosition )
{
    return *(Solid**)GetUserDataFromLua(l, stackPosition, SOLID_TYPE);
}

Solid* CheckSolidFromLua( lua_State* l, int stackPosition )
{
    return *(Solid**)CheckUserDataFromLua(l, stackPosition, SOLID_TYPE);
}


bool RegisterPhysicsManagerInLua()
{
    if(!RegisterUserDataTypeInLua(COLLISION_SHAPE_TYPE, Lua_CollisionShape_destructor))
        return false;
    if(!RegisterUserDataTypeInLua(SOLID_TYPE, Lua_Solid_destructor))
        return false;

    return
        RegisterFunctionInLua("CreateBoxCollisionShape", Lua_CreateBoxCollisionShape) &&
        RegisterFunctionInLua("CreateSphereCollisionShape", Lua_CreateSphereCollisionShape) &&
        RegisterFunctionInLua("CreateCompoundCollisionShape", Lua_CreateCompoundCollisionShape) &&
        RegisterFunctionInLua("CreateSolid", Lua_CreateSolid) &&
        RegisterFunctionInLua("GetSolidMass", Lua_GetSolidMass) &&
        RegisterFunctionInLua("SetSolidMass", Lua_SetSolidMass) &&
        RegisterFunctionInLua("GetSolidPosition", Lua_GetSolidPosition) &&
        RegisterFunctionInLua("GetSolidRotation", Lua_GetSolidRotation);
}
