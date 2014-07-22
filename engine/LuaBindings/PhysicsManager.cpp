#include <string.h> // memset

#include "../Lua.h"
#include "../PhysicsManager.h"
#include "PhysicsManager.h"


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
    Solid* solid = CreateSolid();
    if(solid &&
       CopyUserDataToLua(l, SOLID_TYPE, sizeof(solid), &solid))
    {
        ReferenceSolid(solid);
        return 1;
    }
    else
    {
        lua_pop(l, 1);
        luaL_error(l, "Can't create more solids.");
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

bool RegisterPhysicsManagerInLua()
{
    if(!RegisterUserDataTypeInLua(SOLID_TYPE, Lua_Solid_destructor))
        return false;

    return
        RegisterFunctionInLua("CreateSolid", Lua_CreateSolid) &&
        RegisterFunctionInLua("GetSolidPosition", Lua_GetSolidPosition) &&
        RegisterFunctionInLua("GetSolidRotation", Lua_GetSolidRotation);
}

Solid* GetSolidFromLua( lua_State* l, int stackPosition )
{
    return *(Solid**)GetUserDataFromLua(l, stackPosition, SOLID_TYPE);
}

Solid* CheckSolidFromLua( lua_State* l, int stackPosition )
{
    return *(Solid**)CheckUserDataFromLua(l, stackPosition, SOLID_TYPE);
}
