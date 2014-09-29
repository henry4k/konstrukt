#include "../Lua.h"
#include "../Texture.h"
#include "Texture.h"


static int ReadTextureOption( lua_State* l, int stackPosition )
{
    static const char* optionNames[] =
    {
        "mipmap",
        "filter",
        "clamp",
        NULL
    };

    static int optionMap[] =
    {
        TEX_MIPMAP,
        TEX_FILTER,
        TEX_CLAMP
    };

    const int index = luaL_checkoption(l, stackPosition, NULL, optionNames);
    return optionMap[index];
}

static int ReadTextureOptions( lua_State* l, int startArgument )
{
    int options = 0;
    const int argc = lua_gettop(l);
    for(int i = startArgument; i <= argc; i++)
        options |= ReadTextureOption(l, i);
    return options;
}

static int Lua_Load2dTexture( lua_State* l )
{
    const char* vfsPath = luaL_checkstring(l, 1);
    const int options = ReadTextureOptions(l, 2);

    Texture* texture = Load2dTexture(options, vfsPath);
    if(texture)
    {
        PushPointerToLua(l, texture);
        ReferenceTexture(texture);
        return 1;
    }
    else
    {
        luaL_error(l, "Failed to create texture!");
        return 0;
    }
}

static int Lua_LoadCubeTexture( lua_State* l )
{
    const char* vfsPathTemplate = luaL_checkstring(l, 1);
    const int options = ReadTextureOptions(l, 2);

    Texture* texture = LoadCubeTexture(options, vfsPathTemplate);
    if(texture)
    {
        PushPointerToLua(l, texture);
        ReferenceTexture(texture);
        return 1;
    }
    else
    {
        luaL_error(l, "Failed to create texture!");
        return 0;
    }
}

static int Lua_DestroyTexture( lua_State* l )
{
    Texture* texture = CheckTextureFromLua(l, 1);
    ReleaseTexture(texture);
    return 0;
}

Texture* GetTextureFromLua( lua_State* l, int stackPosition )
{
    return (Texture*)GetPointerFromLua(l, stackPosition);
}

Texture* CheckTextureFromLua( lua_State* l, int stackPosition )
{
    return (Texture*)CheckPointerFromLua(l, stackPosition);
}

bool RegisterTextureInLua()
{
    return
        RegisterFunctionInLua("Load2dTexture", Lua_Load2dTexture) &&
        RegisterFunctionInLua("LoadCubeTexture", Lua_LoadCubeTexture) &&
        RegisterFunctionInLua("DestroyTexture", Lua_DestroyTexture);
}
