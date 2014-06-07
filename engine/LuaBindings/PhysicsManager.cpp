#include <string.h> // memset

#include "../Lua.h"
#include "../PhysicsManager.h"
#include "PhysicsManager.h"


const char* SOLID_TYPE = "Solid";

int Lua_Solid_destructor( lua_State* l )
{
    Solid* solid =
        reinterpret_cast<Solid*>(lua_touserdata(l, 1));
    FreeSolid(solid);
    return 0;
}

int Lua_CreateSolid( lua_State* l )
{
    Solid* solid = CreateSolid();
    if(solid)
    {
        if(CopyUserDataToLua(l, SOLID_TYPE, sizeof(solid), &solid))
            return 1;
        else
            FreeSolid(solid);
    }
    luaL_error(l, "Can't create more solids.");
    return 0;
}

int Lua_FreeSolid( lua_State* l )
{
    Solid* solid = CheckSolidFromLua(l, 1);
    FreeSolid(solid);
    return 0;
}

bool RegisterPhysicsManagerInLua()
{
    if(!RegisterUserDataTypeInLua(SOLID_TYPE, Lua_Solid_destructor))
        return false;

    return
        RegisterFunctionInLua("CreateSolid", Lua_CreateSolid) &&
        RegisterFunctionInLua("FreeSolid", Lua_FreeSolid);
}

Solid* GetSolidFromLua( lua_State* l, int stackPosition )
{
    return *(Solid**)GetUserDataFromLua(l, stackPosition, SOLID_TYPE);
}

Solid* CheckSolidFromLua( lua_State* l, int stackPosition )
{
    return *(Solid**)CheckUserDataFromLua(l, stackPosition, SOLID_TYPE);
}
