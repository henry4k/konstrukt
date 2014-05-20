#include "../Lua.h"
#include "../Effects.h"


int Lua_SetLight( lua_State* l )
{
    const glm::vec3 ambient(
        luaL_checknumber(l, 1),
        luaL_checknumber(l, 2),
        luaL_checknumber(l, 3)
    );

    const glm::vec3 diffuse(
        luaL_checknumber(l, 4),
        luaL_checknumber(l, 5),
        luaL_checknumber(l, 6)
    );

    const glm::vec3 direction(
        luaL_checknumber(l, 7),
        luaL_checknumber(l, 8),
        luaL_checknumber(l, 9)
    );

    SetLight(ambient, diffuse, direction);
    return 0;
}

AutoRegisterInLua()
{
    return
        RegisterFunctionInLua("SetLight", Lua_SetLight);
}
