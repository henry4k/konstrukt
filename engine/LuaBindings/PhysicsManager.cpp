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

int Lua_CreateSphereCollisionShape( lua_State* l )
{
    const float radius = luaL_checknumber(l, 1);

    CollisionShape* shape = CreateSphereCollisionShape(radius);
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
    CollisionShape* shape = CheckCollisionShapeFromLua(l, 1);

    Solid* solid = CreateSolid(shape);
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

static int Lua_GetSolidPosition( lua_State* l )
{
    const Solid* solid = CheckSolidFromLua(l, 2);
    const glm::vec3 position = GetSolidPosition(solid);
    lua_pushnumber(l, position[0]);
    lua_pushnumber(l, position[1]);
    lua_pushnumber(l, position[2]);
    return 3;
}

static int Lua_GetSolidRotation( lua_State* l )
{
    const Solid* solid = CheckSolidFromLua(l, 2);
    const glm::vec3 rotation = GetSolidRotation(solid);
    lua_pushnumber(l, rotation[0]);
    lua_pushnumber(l, rotation[1]);
    lua_pushnumber(l, rotation[2]);
    return 3;
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
        RegisterFunctionInLua("CreateSphereCollisionShape", Lua_CreateSphereCollisionShape) &&
        RegisterFunctionInLua("CreateSolid", Lua_CreateSolid) &&
        RegisterFunctionInLua("GetSolidPosition", Lua_GetSolidPosition) &&
        RegisterFunctionInLua("GetSolidRotation", Lua_GetSolidRotation);
}
