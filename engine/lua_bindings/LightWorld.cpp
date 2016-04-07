#include <string.h> // memset

#include "../Lua.h"
//#include "../Shader.h"
#include "../LightWorld.h"
#include "Math.h"
#include "PhysicsManager.h"
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
    Solid* target = GetSolidFromLua(l, 2);
    const int flags = CheckTransformationFlagsFromLua(l, 3);
    SetLightAttachmentTarget(light, target, flags);
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

static int Lua_SetLightUniform( lua_State* l )
{
    Light* light = CheckLightFromLua(l, 1);
    const char* name = luaL_checkstring(l, 2);

    static const char* types[] =
    {
        "int",
        "float",
        "vec3",
        "mat3",
        "mat4",
        NULL
    };
    const UniformType type = (UniformType)luaL_checkoption(l, 3, NULL, types);

    UniformValue value;
    switch(type)
    {
        case SAMPLER_UNIFORM:
        case INT_UNIFORM:
            value.i = (int)luaL_checknumber(l, 4);
            SetLightUniform(light, name, type, &value);
            break;

        case FLOAT_UNIFORM:
            value.f = luaL_checknumber(l, 4);
            SetLightUniform(light, name, type, &value);
            break;

        case VEC3_UNIFORM:
            value.vec3._[0] = luaL_checknumber(l, 4);
            value.vec3._[1] = luaL_checknumber(l, 5);
            value.vec3._[2] = luaL_checknumber(l, 6);
            SetLightUniform(light, name, type, &value);
            break;

        case MAT3_UNIFORM:
            return luaL_argerror(l, 3, "Mat3 is not supported by the Lua API.");

        case MAT4_UNIFORM:
            const Mat4* m = CheckMatrix4FromLua(l, 4);
            SetLightUniform(light, name, type, (const UniformValue*)&m);
            break;
    }

    return 0;
}

static int Lua_UnsetLightUniform( lua_State* l )
{
    Light* light = CheckLightFromLua(l, 1);
    const char* uniformName = luaL_checkstring(l, 2);
    UnsetLightUniform(light, uniformName);
    return 0;
}

Light* GetLightFromLua( lua_State* l, int stackPosition )
{
    return (Light*)GetPointerFromLua(l, stackPosition);
}

Light* CheckLightFromLua( lua_State* l, int stackPosition )
{
    return (Light*)CheckPointerFromLua(l, stackPosition);
}

bool RegisterLightWorldInLua()
{
    return
        RegisterFunctionInLua("CreateLightWorld", Lua_CreateLightWorld) &&
        RegisterFunctionInLua("DestroyLightWorld", Lua_DestroyLightWorld) &&
        RegisterFunctionInLua("SetMaxActiveLightCount", Lua_SetMaxActiveLightCount) &&

        RegisterFunctionInLua("CreateLight", Lua_CreateLight) &&
        RegisterFunctionInLua("DestroyLight", Lua_DestroyLight) &&
        RegisterFunctionInLua("SetLightAttachmentTarget", Lua_SetLightAttachmentTarget) &&
        RegisterFunctionInLua("SetLightTransformation", Lua_SetLightTransformation) &&
        RegisterFunctionInLua("SetLightValue", Lua_SetLightValue) &&
        RegisterFunctionInLua("SetLightRange", Lua_SetLightRange) &&
        RegisterFunctionInLua("SetLightUniform", Lua_SetLightUniform) &&
        RegisterFunctionInLua("UnsetLightUniform", Lua_UnsetLightUniform);
}
