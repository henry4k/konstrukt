#include "../Lua.h"
#include "../Player.h"


int Lua_GetPlayerPosition( lua_State* l )
{
    using namespace glm;
    vec3 position = GetPlayerPosition();
    lua_pushnumber(l, position.x);
    lua_pushnumber(l, position.y);
    lua_pushnumber(l, position.z);
    return 3;
}

int Lua_GetPlayerOrientation( lua_State* l )
{
    using namespace glm;
    const vec3 orientation = normalize(GetPlayerOrientation() * vec3(0,0,1));
    lua_pushnumber(l, orientation.x);
    lua_pushnumber(l, orientation.y);
    lua_pushnumber(l, orientation.z);
    return 3;
}

bool RegisterPlayerInLua()
{
    return
        RegisterFunctionInLua("GetPlayerPosition", Lua_GetPlayerPosition) &&
        RegisterFunctionInLua("GetPlayerOrientation", Lua_GetPlayerOrientation);
}
