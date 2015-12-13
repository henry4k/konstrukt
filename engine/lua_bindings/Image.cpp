#include "../Lua.h"
#include "../Image.h"
#include "Image.h"


static const char* IMAGE_TYPE = "Image";

static int Lua_FreeImage( lua_State* l )
{
    Image* image = CheckImageFromLua(l, 1);
    FreeImage(image);
    return 0;
}

static int Lua_LoadImage( lua_State* l )
{
    const char* vfsPath = luaL_checkstring(l, 1);

    Image* image = (Image*)PushUserDataToLua(l, IMAGE_TYPE, sizeof(Image));
    if(LoadImage(image, vfsPath))
    {
        return 1;
    }
    else
    {
        lua_pop(l, 1); // Pop image
        luaL_error(l, "Failed to load image!");
        return 0;
    }
}

static int Lua_MultiplyImageRgbByAlpha( lua_State* l )
{
    Image* image = CheckImageFromLua(l, 1);
    MultiplyImageRgbByAlpha(image);
    return 0;
}

static int Lua_CreateResizedImage( lua_State* l )
{
    const Image* input = CheckImageFromLua(l, 1);
    const int width    = luaL_checkinteger(l, 2);
    const int height   = luaL_checkinteger(l, 3);

    Image* output = (Image*)PushUserDataToLua(l, IMAGE_TYPE, sizeof(Image));
    if(CreateResizedImage(output, input, width, height))
    {
        return 1;
    }
    else
    {
        lua_pop(l, 1); // Pop image
        luaL_error(l, "Failed create resized image!");
        return 0;
    }
}

static int Lua_GetImageMetadata( lua_State* l )
{
    const Image* image = CheckImageFromLua(l, 1);
    lua_pushinteger(l, image->width);
    lua_pushinteger(l, image->height);
    lua_pushinteger(l, image->channelCount);
    return 3;
}

Image* GetImageFromLua( lua_State* l, int stackPosition )
{
    return (Image*)GetUserDataFromLua(l, stackPosition, IMAGE_TYPE);
}

Image* CheckImageFromLua( lua_State* l, int stackPosition )
{
    return (Image*)CheckUserDataFromLua(l, stackPosition, IMAGE_TYPE);
}

bool RegisterImageInLua()
{
    return
        RegisterUserDataTypeInLua(IMAGE_TYPE, Lua_FreeImage) &&
        RegisterFunctionInLua("LoadImage", Lua_LoadImage) &&
        RegisterFunctionInLua("MultiplyImageRgbByAlpha", Lua_MultiplyImageRgbByAlpha) &&
        RegisterFunctionInLua("CreateResizedImage", Lua_CreateResizedImage) &&
        RegisterFunctionInLua("GetImageMetadata", Lua_GetImageMetadata);
}
