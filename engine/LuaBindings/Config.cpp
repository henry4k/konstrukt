#include "../Lua.h"
#include "../Config.h"


int Lua_GetConfigValue( lua_State* l )
{
    const char* key = luaL_checkstring(l, 1);

    const char* stringValue = GetConfigString(key, NULL);
    if(stringValue == NULL)
    {
        lua_pushvalue(l, 2); // push default value
        return 1;
    }

    switch(lua_type(l, 2)) // use type of default value to determine return type
    {
        case LUA_TSTRING:
            lua_pushstring(l, stringValue);
            return 1;

        case LUA_TNUMBER:
            lua_pushnumber(l, GetConfigFloat(key, 0));
            return 1;

        case LUA_TBOOLEAN:
            lua_pushboolean(l, GetConfigBool(key, false));
            return 1;

        default:
            luaL_error(l, "Unsupported type. (Only string, number and bool!)");
            return 0;
    }
}

AutoRegisterInLua()
{
    return
        RegisterFunctionInLua("GetConfigValue", Lua_GetConfigValue);
}
