#include "../Lua.h"
#include "../Texture.h"
#include "Texture.h"


static const char* TEXTURE_TYPE = "Texture";

static int Lua_Texture_destructor( lua_State* l )
{
    Texture* texture = CheckTextureFromLua(l, 1);
    ReleaseTexture(texture);
    return 0;
}

static const char* TextureOptionNames[] =
{
    "mipmap",
    "filter",
    "clamp",
    NULL
};

static int TextureOptionMap[] =
{
    TEX_MIPMAP,
    TEX_FILTER,
    TEX_CLAMP
};

static int ReadTextureOption( lua_State* l, int stackPosition )
{
    const int index = luaL_checkoption(l, stackPosition, NULL, TextureOptionNames);
    return TextureOptionMap[index];
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
    const char* fileName = luaL_checkstring(l, 1);
    const int options = ReadTextureOptions(l, 2);

    Texture* texture = Load2dTexture(options, fileName);
    if(texture &&
       CopyUserDataToLua(l, TEXTURE_TYPE, sizeof(texture), &texture))
    {
        ReferenceTexture(texture);
        return 1;
    }
    else
    {
        lua_pop(l, 1);
        luaL_error(l, "Failed to create texture!");
        return 0;
    }
}

static int Lua_LoadCubeTexture( lua_State* l )
{
    const char* filePrefix = luaL_checkstring(l, 1);
    const int options = ReadTextureOptions(l, 2);

    Texture* texture = LoadCubeTexture(options, filePrefix);
    if(texture &&
       CopyUserDataToLua(l, TEXTURE_TYPE, sizeof(texture), &texture))
    {
        ReferenceTexture(texture);
        return 1;
    }
    else
    {
        lua_pop(l, 1);
        luaL_error(l, "Failed to create texture!");
        return 0;
    }
}

bool RegisterTextureInLua()
{
    if(!RegisterUserDataTypeInLua(TEXTURE_TYPE, Lua_Texture_destructor))
        return false;

    return
        RegisterFunctionInLua("Load2dTexture", Lua_Load2dTexture) &&
        RegisterFunctionInLua("LoadCubeTexture", Lua_LoadCubeTexture);
}

Texture* GetTextureFromLua( lua_State* l, int stackPosition )
{
    return *(Texture**)GetUserDataFromLua(l, stackPosition, TEXTURE_TYPE);
}

Texture* CheckTextureFromLua( lua_State* l, int stackPosition )
{
    return *(Texture**)CheckUserDataFromLua(l, stackPosition, TEXTURE_TYPE);
}
