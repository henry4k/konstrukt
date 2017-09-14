#include <string.h> // memset

#include "../Lua.h"
#include "../LightWorld.h"
#include "Math.h"
#include "AttachmentTarget.h"
#include "Shader.h"
#include "LightWorld.h"


// ---- LightWorld ----

static int Lua_CreateLightWorld( lua_State* l )
{
    const char* lightCountUniformName = luaL_checkstring(l, 1);
    const char* lightPositionName     = luaL_checkstring(l, 2);
    LightWorld* world = CreateLightWorld(lightCountUniformName,
                                         lightPositionName);
    if(world)
    {
        PushPointerToLua(l, world);
        ReferenceLightWorld(world);
        return 1;
    }
    else
    {
        return luaL_error(l, "Can't create more light worlds.");
    }
}

static int Lua_DestroyLightWorld( lua_State* l )
{
    LightWorld* world = CheckLightWorldFromLua(l, 1);
    ReleaseLightWorld(world);
    return 0;
}

static int Lua_SetMaxActiveLightCount( lua_State* l )
{
    LightWorld* world = CheckLightWorldFromLua(l, 1);
    const int count = luaL_checkinteger(l, 2);
    SetMaxActiveLightCount(world, count);
    return 0;
}

static int Lua_GetLightWorldShaderVariableSet( lua_State* l )
{
    LightWorld* world = CheckLightWorldFromLua(l, 1);
    PushShaderVariableSetToLua(l, GetLightWorldShaderVariableSet(world));
    return 1;
}

static int Lua_GetLightWorldUnusedLightShaderVariableSet( lua_State* l )
{
    LightWorld* world = CheckLightWorldFromLua(l, 1);
    PushShaderVariableSetToLua(l, GetLightWorldUnusedLightShaderVariableSet(world));
    return 1;
}

LightWorld* GetLightWorldFromLua( lua_State* l, int stackPosition )
{
    return (LightWorld*)GetPointerFromLua(l, stackPosition);
}

LightWorld* CheckLightWorldFromLua( lua_State* l, int stackPosition )
{
    return (LightWorld*)CheckPointerFromLua(l, stackPosition);
}


// ---- Light ----

static int Lua_CreateLight( lua_State* l )
{
    LightWorld* world = CheckLightWorldFromLua(l, 1);

    static const char* lightTypes[] =
    {
        "global",
        "point",
        NULL
    };

    const LightType type = (LightType)luaL_checkoption(l, 2, NULL, lightTypes);

    Light* light = CreateLight(world, type);
    if(light)
    {
        PushPointerToLua(l, light);
        ReferenceLight(light);
        return 1;
    }
    else
    {
        return luaL_error(l, "Can't create more lights.");
    }
}

static int Lua_DestroyLight( lua_State* l )
{
    Light* light = CheckLightFromLua(l, 1);
    ReleaseLight(light);
    return 0;
}

static int Lua_SetLightAttachmentTarget( lua_State* l )
{
    Light* light = CheckLightFromLua(l, 1);
    const AttachmentTarget* target = GetAttachmentTargetFromLua(l, 2);
    SetLightAttachmentTarget(light, target);
    return 0;
}

static int Lua_SetLightTransformation( lua_State* l )
{
    Light* light = CheckLightFromLua(l, 1);
    const Mat4* transformation = CheckMatrix4FromLua(l, 2);
    SetLightTransformation(light, *transformation);
    return 0;
}

static int Lua_SetLightValue( lua_State* l )
{
    Light* light = CheckLightFromLua(l, 1);
    const float value = luaL_checknumber(l, 2);
    SetLightValue(light, value);
    return 0;
}

static int Lua_SetLightRange( lua_State* l )
{
    Light* light = CheckLightFromLua(l, 1);
    const float range = luaL_checknumber(l, 2);
    SetLightRange(light, range);
    return 0;
}

static int Lua_GetLightShaderVariableSet( lua_State* l )
{
    Light* light = CheckLightFromLua(l, 1);
    PushShaderVariableSetToLua(l, GetLightShaderVariableSet(light));
    return 1;
}

Light* GetLightFromLua( lua_State* l, int stackPosition )
{
    return (Light*)GetPointerFromLua(l, stackPosition);
}

Light* CheckLightFromLua( lua_State* l, int stackPosition )
{
    return (Light*)CheckPointerFromLua(l, stackPosition);
}

void RegisterLightWorldInLua()
{
    RegisterFunctionInLua("CreateLightWorld", Lua_CreateLightWorld);
    RegisterFunctionInLua("DestroyLightWorld", Lua_DestroyLightWorld);
    RegisterFunctionInLua("SetMaxActiveLightCount", Lua_SetMaxActiveLightCount);
    RegisterFunctionInLua("GetLightWorldShaderVariableSet", Lua_GetLightWorldShaderVariableSet);
    RegisterFunctionInLua("GetLightWorldUnusedLightShaderVariableSet", Lua_GetLightWorldUnusedLightShaderVariableSet);

    RegisterFunctionInLua("CreateLight", Lua_CreateLight);
    RegisterFunctionInLua("DestroyLight", Lua_DestroyLight);
    RegisterFunctionInLua("SetLightAttachmentTarget", Lua_SetLightAttachmentTarget);
    RegisterFunctionInLua("SetLightTransformation", Lua_SetLightTransformation);
    RegisterFunctionInLua("SetLightValue", Lua_SetLightValue);
    RegisterFunctionInLua("SetLightRange", Lua_SetLightRange);
    RegisterFunctionInLua("GetLightShaderVariableSet", Lua_GetLightShaderVariableSet);
}
