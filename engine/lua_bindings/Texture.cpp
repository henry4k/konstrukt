#include "../Lua.h"
#include "../Image.h"
#include "../Texture.h"
#include "Image.h"
#include "Texture.h"


static int ReadTextureOption( lua_State* l, int stackPosition )
{
    static const char* optionNames[] =
    {
        "mipmap",
        "filter",
        "clamp",
        "srgb",
        NULL
    };

    static int optionMap[] =
    {
        TEX_MIPMAP,
        TEX_FILTER,
        TEX_CLAMP,
        TEX_SRGB
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

static int Lua_Create2dTexture( lua_State* l )
{
    const Image* image = CheckImageFromLua(l, 1);
    const int options = ReadTextureOptions(l, 2);

    Texture* texture = Create2dTexture(image, options);
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

static int Lua_CreateCubeTexture( lua_State* l )
{
    const Image* images[6];
    for(int i = 0; i < 6; i++)
    {
        lua_rawgeti(l, 1, i+1);
        images[i] = CheckImageFromLua(l, -1);
        lua_pop(l, 1);
    }
    const int options = ReadTextureOptions(l, 2);

    Texture* texture = CreateCubeTexture(images, options);
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

void RegisterTextureInLua()
{
    RegisterFunctionInLua("Create2dTexture", Lua_Create2dTexture);
    RegisterFunctionInLua("CreateCubeTexture", Lua_CreateCubeTexture);
    RegisterFunctionInLua("DestroyTexture", Lua_DestroyTexture);
}
