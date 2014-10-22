#include "../Lua.h"
#include "../RenderTarget.h"
#include "Camera.h"
#include "Shader.h"
#include "Texture.h"
#include "RenderTarget.h"


static int Lua_GetDefaultRenderTarget( lua_State* l )
{
    RenderTarget* target = GetDefaultRenderTarget();
    PushPointerToLua(l, target);
    // Note that no reference is put on the render target here.
    // This is done intentionally, since you can't destroy the default render
    // target.
    return 1;
}

static int Lua_CreateTextureRenderTarget( lua_State* l )
{
    Texture* texture = CheckTextureFromLua(l, 1);
    RenderTarget* target = CreateTextureRenderTarget(texture);
    if(target)
    {
        PushPointerToLua(l, target);
        ReferenceRenderTarget(target);
        return 1;
    }
    else
    {
        return luaL_error(l, "Can't create render target.");
    }
}

static int Lua_DestroyRenderTarget( lua_State* l )
{
    RenderTarget* target = CheckRenderTargetFromLua(l, 1);
    ReleaseRenderTarget(target);
    return 0;
}

static int Lua_SetRenderTargetCamera( lua_State* l )
{
    RenderTarget* target = CheckRenderTargetFromLua(l, 1);
    Camera* camera = CheckCameraFromLua(l, 2);
    int layer = luaL_checkinteger(l, 3);
    SetRenderTargetCamera(target, camera, layer);
    return 0;
}

static int Lua_SetRenderTargetShaderProgramSet( lua_State* l )
{
    RenderTarget* target = CheckRenderTargetFromLua(l, 1);
    ShaderProgramSet* programSet = CheckShaderProgramSetFromLua(l, 2);
    SetRenderTargetShaderProgramSet(target, programSet);
    return 0;
}

RenderTarget* GetRenderTargetFromLua( lua_State* l, int stackPosition )
{
    return (RenderTarget*)GetPointerFromLua(l, stackPosition);
}

RenderTarget* CheckRenderTargetFromLua( lua_State* l, int stackPosition )
{
    return (RenderTarget*)CheckPointerFromLua(l, stackPosition);
}

bool RegisterRenderTargetInLua()
{
    return
        RegisterFunctionInLua("GetDefaultRenderTarget", Lua_GetDefaultRenderTarget) &&
        RegisterFunctionInLua("CreateTextureRenderTarget", Lua_CreateTextureRenderTarget) &&
        RegisterFunctionInLua("DestroyRenderTarget", Lua_DestroyRenderTarget) &&
        RegisterFunctionInLua("SetRenderTargetCamera", Lua_SetRenderTargetCamera) &&
        RegisterFunctionInLua("SetRenderTargetShaderProgramSet", Lua_SetRenderTargetShaderProgramSet);
}
