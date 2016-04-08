#include "../Lua.h"
#include "../Camera.h"
#include "Math.h"
#include "PhysicsManager.h"
#include "ModelWorld.h"
#include "LightWorld.h"
#include "Shader.h"
#include "Camera.h"


static int Lua_CreateCamera( lua_State* l )
{
    ModelWorld* modelWorld = CheckModelWorldFromLua(l, 1);
    LightWorld* lightWorld = GetLightWorldFromLua(l, 2);
    Camera* camera = CreateCamera(modelWorld, lightWorld);
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
    Solid* attachmentTarget = GetSolidFromLua(l, 2);
    const int attachmentFlags = CheckTransformationFlagsFromLua(l, 3);
    SetCameraAttachmentTarget(camera, attachmentTarget, attachmentFlags);
    return 0;
}

static int Lua_SetCameraModelTransformation( lua_State* l )
{
    Camera* camera = CheckCameraFromLua(l, 1);
    const Mat4* transformation = CheckMatrix4FromLua(l, 2);
    SetCameraModelTransformation(camera, *transformation);
    return 0;
}

static int Lua_SetCameraViewTransformation( lua_State* l )
{
    Camera* camera = CheckCameraFromLua(l, 1);
    const Mat4* transformation = CheckMatrix4FromLua(l, 2);
    SetCameraViewTransformation(camera, *transformation);
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

static int Lua_SetCameraProjectionType( lua_State* l )
{
    static const char* typeNames[] =
    {
        "perspective",
        "orthographic",
        NULL
    };

    static CameraProjectionType typeMap[] =
    {
        CAMERA_PERSPECTIVE_PROJECTION,
        CAMERA_ORTHOGRAPHIC_PROJECTION
    };

    Camera* camera = CheckCameraFromLua(l, 1);
    const CameraProjectionType type =
        typeMap[luaL_checkoption(l, 2, NULL, typeNames)];

    SetCameraProjectionType(camera, type);
    return 0;
}

static int Lua_SetCameraFieldOfView( lua_State* l )
{
    Camera* camera = CheckCameraFromLua(l, 1);
    const float fov = luaL_checknumber(l, 2);
    SetCameraFieldOfView(camera, fov);
    return 0;
}

static int Lua_SetCameraScale( lua_State* l )
{
    Camera* camera = CheckCameraFromLua(l, 1);
    const float scale = luaL_checknumber(l, 2);
    SetCameraScale(camera, scale);
    return 0;
}

static int Lua_GetCameraShaderVariableSet( lua_State* l )
{
    Camera* camera = CheckCameraFromLua(l, 1);
    PushShaderVariableSetToLua(l, GetCameraShaderVariableSet(camera));
    return 1;
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
        RegisterFunctionInLua("SetCameraModelTransformation", Lua_SetCameraModelTransformation) &&
        RegisterFunctionInLua("SetCameraViewTransformation", Lua_SetCameraViewTransformation) &&
        RegisterFunctionInLua("SetCameraNearAndFarPlanes", Lua_SetCameraNearAndFarPlanes) &&
        RegisterFunctionInLua("SetCameraProjectionType", Lua_SetCameraProjectionType) &&
        RegisterFunctionInLua("SetCameraFieldOfView", Lua_SetCameraFieldOfView) &&
        RegisterFunctionInLua("SetCameraScale", Lua_SetCameraScale) &&
        RegisterFunctionInLua("GetCameraShaderVariableSet", Lua_GetCameraShaderVariableSet);
}
