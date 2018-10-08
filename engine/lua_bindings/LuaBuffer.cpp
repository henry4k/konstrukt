#include "../Lua.h"
#include "../LuaBuffer.h"
#include "LuaBuffer.h"

#if LUA_VERSION_NUM <= 502
#define lua_isinteger(...) 0
#endif

static int Lua_CreateLuaBuffer( lua_State* l )
{
    static const char* typeNames[] =
    {
        "native", // NATIVE_LUA_BUFFER
        NULL
    };

    const LuaBufferType type = (LuaBufferType)luaL_checkoption(l, 1, NULL, typeNames);
    LuaBuffer* buffer = CreateLuaBuffer(type);

    const int top = lua_gettop(l);
    for(int i = 2; i < top; i++)
    {
        switch(lua_type(l, i))
        {
            case LUA_TNIL:
                AddNilToLuaBuffer(buffer);
                break;

            case LUA_TBOOLEAN:
                AddNumberToLuaBuffer(buffer, lua_toboolean(l, i) != 0);
                break;

            case LUA_TNUMBER:
                if(lua_isinteger(l, i))
                    AddIntegerToLuaBuffer(buffer, lua_tointeger(l, i));
                else
                    AddNumberToLuaBuffer(buffer, lua_tonumber(l, i));
                break;

            case LUA_TSTRING:
            {
                size_t length;
                const char* string = lua_tolstring(l, i, &length);
                AddStringToLuaBuffer(buffer, string, length, 0);
                break;
            }

            default:
                return luaL_error(l, "Can't serialize %s values (parameter %d)",
                                     lua_typename(l, lua_type(l, i)),
                                     i);
        }
    }

    PushPointerToLua(l, buffer);
    ReferenceLuaBuffer(buffer);

    return 1;
}

static int Lua_DestroyLuaBuffer( lua_State* l )
{
    LuaBuffer* buffer = CheckLuaBufferFromLua(l, 1);
    ReleaseLuaBuffer(buffer);
    return 0;
}

LuaBuffer* GetLuaBufferFromLua( lua_State* l, int stackPosition )
{
    return (LuaBuffer*)GetPointerFromLua(l, stackPosition);
}

LuaBuffer* CheckLuaBufferFromLua( lua_State* l, int stackPosition )
{
    return (LuaBuffer*)CheckPointerFromLua(l, stackPosition);
}

void RegisterLuaBufferInLua()
{
    RegisterFunctionInLua("CreateLuaBuffer", Lua_CreateLuaBuffer);
    RegisterFunctionInLua("DestroyLuaBuffer", Lua_DestroyLuaBuffer);
}
