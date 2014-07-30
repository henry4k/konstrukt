#include "../Lua.h"
#include "../RenderManager.h"
#include "Math.h"
#include "PhysicsManager.h"
#include "RenderManager.h"


static int Lua_SetCameraAttachmentTarget( lua_State* l )
{
    Solid* target = CheckSolidFromLua(l, 1);
    SetCameraAttachmentTarget(target);
    return 0;
}

static int Lua_SetCameraViewTransformation( lua_State* l )
{
    const glm::mat4* transformation = CheckMatrix4FromLua(l, 1);
    SetCameraViewTransformation(*transformation);
    return 0;
}

static int Lua_SetCameraFieldOfView( lua_State* l )
{
    const float fov = luaL_checknumber(l, 1);
    SetCameraFieldOfView(fov);
    return 0;
}

bool RegisterRenderManagerInLua()
{
    return
        RegisterFunctionInLua("SetCameraAttachmentTarget", Lua_SetCameraAttachmentTarget) &&
        RegisterFunctionInLua("SetCameraViewTransformation", Lua_SetCameraViewTransformation) &&
        RegisterFunctionInLua("SetCameraFieldOfView", Lua_SetCameraFieldOfView);
}