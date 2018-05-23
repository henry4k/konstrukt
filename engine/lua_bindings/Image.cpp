#include "../Lua.h"
#include "../Image.h"
#include "Image.h"
#include "JobManager.h"


static void PushImageToLua( lua_State* l, Image* image );

static int Lua_BeginLoadingImage( lua_State* l )
{
    const char* vfsPath = luaL_checkstring(l, 1);
    PushJobToLua(l, BeginLoadingImage(vfsPath));
    return 1;
}

static int Lua_BeginResizingImage( lua_State* l )
{
    Image* input = CheckImageFromLua(l, 1);
    const int width = luaL_checkinteger(l, 2);
    const int height = luaL_checkinteger(l, 3);
    PushJobToLua(l, BeginResizingImage(input, width, height));
    return 1;
}

static int Lua_GetCreatedImage( lua_State* l )
{
    const JobId job = CheckJobFromLua(l, 1);
    PushImageToLua(l, GetCreatedImage(job));
    return 1;
}

static int Lua_MultiplyImageRgbByAlpha( lua_State* l )
{
    Image* image = CheckImageFromLua(l, 1);
    PushJobToLua(l, MultiplyImageRgbByAlpha_(image));
    return 1;
}

static int Lua_GetImageMetadata( lua_State* l )
{
    const Image* image = CheckImageFromLua(l, 1);
    lua_pushinteger(l, GetImageWidth(image));
    lua_pushinteger(l, GetImageHeight(image));
    lua_pushinteger(l, GetImageChannelCount(image));
    return 3;
}

static void PushImageToLua( lua_State* l, Image* image )
{
    PushPointerToLua(l, image);
}

Image* GetImageFromLua( lua_State* l, int stackPosition )
{
    return (Image*)GetPointerFromLua(l, stackPosition);
}

Image* CheckImageFromLua( lua_State* l, int stackPosition )
{
    return (Image*)CheckPointerFromLua(l, stackPosition);
}

void RegisterImageInLua()
{
    RegisterFunctionInLua("BeginLoadingImage", Lua_BeginLoadingImage);
    RegisterFunctionInLua("BeginResizingImage", Lua_BeginResizingImage);
    RegisterFunctionInLua("GetCreatedImage", Lua_GetCreatedImage);
    RegisterFunctionInLua("MultiplyImageRgbByAlpha", Lua_MultiplyImageRgbByAlpha);
    RegisterFunctionInLua("GetImageMetadata", Lua_GetImageMetadata);
}
