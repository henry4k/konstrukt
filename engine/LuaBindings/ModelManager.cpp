#include <string.h> // memset

#include "../Lua.h"
#include "../Mesh.h"
#include "../Shader.h"
#include "../ModelManager.h"
#include "Math.h"
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include "PhysicsManager.h"
#include "ModelManager.h"


static int Lua_CreateModel( lua_State* l )
{
    static const char* stages[] =
    {
        "background",
        "world",
        "hud",
        NULL
    };

    const ModelStage stage = (ModelStage)luaL_checkoption(l, 1, NULL, stages);
    ShaderProgram* program = CheckShaderProgramFromLua(l, 2);

    Model* model = CreateModel(stage, program);
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
    Solid* target = CheckSolidFromLua(l, 2);
    SetModelAttachmentTarget(model, target);
    return 0;
}

static int Lua_SetModelTransformation( lua_State* l )
{
    Model* model = CheckModelFromLua(l, 1);
    const glm::mat4* transformation = CheckMatrix4FromLua(l, 2);
    SetModelTransformation(model, *transformation);
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
    Texture* texture = CheckTextureFromLua(l, 3);
    SetModelTexture(model, unit, texture);
    return 0;
}

void SetModelUniform( Model* model, const char* name, UniformValue* value );

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

bool RegisterModelManagerInLua()
{
    return
        RegisterFunctionInLua("CreateModel", Lua_CreateModel) &&
        RegisterFunctionInLua("DestroyModel", Lua_DestroyModel) &&
        RegisterFunctionInLua("SetModelAttachmentTarget", Lua_SetModelAttachmentTarget) &&
        RegisterFunctionInLua("SetModelTransformation", Lua_SetModelTransformation) &&
        RegisterFunctionInLua("SetModelMesh", Lua_SetModelMesh) &&
        RegisterFunctionInLua("SetModelTexture", Lua_SetModelTexture) &&
        RegisterFunctionInLua("SetModelUniform", Lua_SetModelUniform) &&
        RegisterFunctionInLua("UnsetModelUniform", Lua_UnsetModelUniform);
}
