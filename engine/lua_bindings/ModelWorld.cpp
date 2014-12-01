#include <string.h> // memset

#include "../Lua.h"
#include "../Mesh.h"
#include "../Shader.h"
#include "../ModelWorld.h"
#include "Math.h"
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include "PhysicsManager.h"
#include "ModelWorld.h"


// ---- ModelWorld ----

static int Lua_CreateModelWorld( lua_State* l )
{
    ModelWorld* world = CreateModelWorld();
    if(world)
    {
        PushPointerToLua(l, world);
        ReferenceModelWorld(world);
        return 1;
    }
    else
    {
        return luaL_error(l, "Can't create more model worlds.");
    }
}

static int Lua_DestroyModelWorld( lua_State* l )
{
    ModelWorld* world = CheckModelWorldFromLua(l, 1);
    ReleaseModelWorld(world);
    return 0;
}

ModelWorld* GetModelWorldFromLua( lua_State* l, int stackPosition )
{
    return (ModelWorld*)GetPointerFromLua(l, stackPosition);
}

ModelWorld* CheckModelWorldFromLua( lua_State* l, int stackPosition )
{
    return (ModelWorld*)CheckPointerFromLua(l, stackPosition);
}


// ---- Model ----

static int Lua_CreateModel( lua_State* l )
{
    ModelWorld* world = CheckModelWorldFromLua(l, 1);

    Model* model = CreateModel(world);
    if(model)
    {
        PushPointerToLua(l, model);
        ReferenceModel(model);
        return 1;
    }
    else
    {
        return luaL_error(l, "Can't create more models.");
    }
}

static int Lua_DestroyModel( lua_State* l )
{
    Model* model = CheckModelFromLua(l, 1);
    ReleaseModel(model);
    return 0;
}

static int Lua_SetModelAttachmentTarget( lua_State* l )
{
    Model* model = CheckModelFromLua(l, 1);
    Solid* target = GetSolidFromLua(l, 2);
    const int flags = CheckTransformationFlagsFromLua(l, 3);
    SetModelAttachmentTarget(model, target, flags);
    return 0;
}

static int Lua_SetModelTransformation( lua_State* l )
{
    Model* model = CheckModelFromLua(l, 1);
    const glm::mat4* transformation = CheckMatrix4FromLua(l, 2);
    SetModelTransformation(model, *transformation);
    return 0;
}

static int Lua_SetModelOverlayLevel( lua_State* l )
{
    Model* model = CheckModelFromLua(l, 1);
    const int level = luaL_checkinteger(l, 2);
    SetModelOverlayLevel(model, level);
    return 0;
}

static int Lua_SetModelMesh( lua_State* l )
{
    Model* model = CheckModelFromLua(l, 1);
    Mesh* mesh = CheckMeshFromLua(l, 2);
    SetModelMesh(model, mesh);
    return 0;
}

static int Lua_SetModelTexture( lua_State* l )
{
    Model* model = CheckModelFromLua(l, 1);
    const int unit = luaL_checkinteger(l, 2);
    Texture* texture = GetTextureFromLua(l, 3);
    SetModelTexture(model, unit, texture);
    return 0;
}

static int Lua_SetModelProgramFamilyList( lua_State* l )
{
    Model* model = CheckModelFromLua(l, 1);
    const char* familyList = luaL_checkstring(l, 2);
    SetModelProgramFamilyList(model, familyList);
    return 0;
}

static int Lua_SetModelUniform( lua_State* l )
{
    Model* model = CheckModelFromLua(l, 1);
    const char* name = luaL_checkstring(l, 2);

    static const char* types[] =
    {
        "int",
        "float",
        "vec3",
        "vec4",
        "mat3",
        "mat4",
        NULL
    };
    const UniformType type = (UniformType)luaL_checkoption(l, 3, NULL, types);

    UniformValue value;
    switch(type)
    {
        case INT_UNIFORM:
            value.i = (int)luaL_checknumber(l, 4);
            SetModelUniform(model, name, type, &value);
            break;

        case FLOAT_UNIFORM:
            value.f = luaL_checknumber(l, 4);
            SetModelUniform(model, name, type, &value);
            break;

        case VEC3_UNIFORM:
            value.data[0] = luaL_checknumber(l, 4);
            value.data[1] = luaL_checknumber(l, 5);
            value.data[2] = luaL_checknumber(l, 6);
            SetModelUniform(model, name, type, &value);
            break;

        case VEC4_UNIFORM:
            value.data[0] = luaL_checknumber(l, 4);
            value.data[1] = luaL_checknumber(l, 5);
            value.data[2] = luaL_checknumber(l, 6);
            value.data[3] = luaL_checknumber(l, 7);
            SetModelUniform(model, name, type, &value);
            break;

        case MAT3_UNIFORM:
            return luaL_argerror(l, 3, "mat3 is not support by the Lua api.");

        case MAT4_UNIFORM:
            const glm::mat4* m = CheckMatrix4FromLua(l, 4);
            SetModelUniform(model, name, type, (const UniformValue*)&m);
            break;
    }

    return 0;
}

static int Lua_UnsetModelUniform( lua_State* l )
{
    Model* model = CheckModelFromLua(l, 1);
    const char* uniformName = luaL_checkstring(l, 2);
    UnsetModelUniform(model, uniformName);
    return 0;
}

Model* GetModelFromLua( lua_State* l, int stackPosition )
{
    return (Model*)GetPointerFromLua(l, stackPosition);
}

Model* CheckModelFromLua( lua_State* l, int stackPosition )
{
    return (Model*)CheckPointerFromLua(l, stackPosition);
}

bool RegisterModelWorldInLua()
{
    return
        RegisterFunctionInLua("CreateModelWorld", Lua_CreateModelWorld) &&
        RegisterFunctionInLua("DestroyModelWorld", Lua_DestroyModelWorld) &&

        RegisterFunctionInLua("CreateModel", Lua_CreateModel) &&
        RegisterFunctionInLua("DestroyModel", Lua_DestroyModel) &&
        RegisterFunctionInLua("SetModelAttachmentTarget", Lua_SetModelAttachmentTarget) &&
        RegisterFunctionInLua("SetModelTransformation", Lua_SetModelTransformation) &&
        RegisterFunctionInLua("SetModelOverlayLevel", Lua_SetModelOverlayLevel) &&
        RegisterFunctionInLua("SetModelMesh", Lua_SetModelMesh) &&
        RegisterFunctionInLua("SetModelTexture", Lua_SetModelTexture) &&
        RegisterFunctionInLua("SetModelProgramFamilyList", Lua_SetModelProgramFamilyList) &&
        RegisterFunctionInLua("SetModelUniform", Lua_SetModelUniform) &&
        RegisterFunctionInLua("UnsetModelUniform", Lua_UnsetModelUniform);
}
