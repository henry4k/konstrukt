#include "../Lua.h"
#include "../Camera.h"
#include "Math.h"
#include "PhysicsManager.h"
#include "ModelWorld.h"
#include "Camera.h"


static int Lua_CreateCamera( lua_State* l )
{
    ModelWorld* world = CheckModelWorldFromLua(l, 1);
    Camera* camera = CreateCamera(world);
    if(camera)
    {
        PushPointerToLua(l, camera);
        ReferenceCamera(camera);
        return 1;
    }
    else
    {
        return luaL_error(l, "Can't create more cameras.");
    }
}

static int Lua_DestroyCamera( lua_State* l )
{
    Camera* camera = CheckCameraFromLua(l, 1);
    ReleaseCamera(camera);
    return 0;
}

static int Lua_SetCameraAttachmentTarget( lua_State* l )
{
    Camera* camera = CheckCameraFromLua(l, 1);
    Solid* attachmentTarget = CheckSolidFromLua(l, 2);
    SetCameraAttachmentTarget(camera, attachmentTarget);
    return 0;
}

static int Lua_SetCameraViewTransformation( lua_State* l )
{
    Camera* camera = CheckCameraFromLua(l, 1);
    const glm::mat4* transformation = CheckMatrix4FromLua(l, 2);
    SetCameraViewTransformation(camera, *transformation);
    return 0;
}

static int Lua_SetCameraFieldOfView( lua_State* l )
{
    Camera* camera = CheckCameraFromLua(l, 1);
    const float fov = luaL_checknumber(l, 2);
    SetCameraFieldOfView(camera, fov);
    return 0;
}

static int Lua_SetCameraNearAndFarPlanes( lua_State* l )
{
    Camera* camera = CheckCameraFromLua(l, 1);
    const float zNear = luaL_checknumber(l, 2);
    const float zFar  = luaL_checknumber(l, 3);
    SetCameraNearAndFarPlanes(camera, zNear, zFar);
    return 0;
}

Camera* GetCameraFromLua( lua_State* l, int stackPosition )
{
    return (Camera*)GetPointerFromLua(l, stackPosition);
}

Camera* CheckCameraFromLua( lua_State* l, int stackPosition )
{
    return (Camera*)CheckPointerFromLua(l, stackPosition);
}

bool RegisterCameraInLua()
{
    return
        RegisterFunctionInLua("CreateCamera", Lua_CreateCamera) &&
        RegisterFunctionInLua("DestroyCamera", Lua_DestroyCamera) &&
        RegisterFunctionInLua("SetCameraAttachmentTarget", Lua_SetCameraAttachmentTarget) &&
        RegisterFunctionInLua("SetCameraViewTransformation", Lua_SetCameraViewTransformation) &&
        RegisterFunctionInLua("SetCameraFieldOfView", Lua_SetCameraFieldOfView) &&
        RegisterFunctionInLua("SetCameraNearAndFarPlanes", Lua_SetCameraNearAndFarPlanes);
}
