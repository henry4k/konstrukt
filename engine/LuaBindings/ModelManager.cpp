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
        luaL_error(l, "Can't create more models.");
        return 0;
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

static int Lua_SetModelUniform( lua_State* l )
{
    Model* model = CheckModelFromLua(l, 1);
    const char* uniformName = luaL_checkstring(l, 2);
    UniformValue uniformValue;
    memset(&uniformValue, 0, sizeof(uniformValue));

    const int argc = lua_gettop(l);
    static const int FIRST_VALUE = 3;

    if(argc < FIRST_VALUE)
    {
        luaL_error(l, "Uniform value missing.");
        return 0;
    }

    if(argc >= FIRST_VALUE+3)
    {
        luaL_error(l, "Too many arguments.");
        return 0;
    }

    if(lua_type(l, FIRST_VALUE) == LUA_TNUMBER)
    {
        // float, vec2, vec3, vec4
        for(int argi = FIRST_VALUE; argi <= argc; argi++)
            uniformValue.data[argi-FIRST_VALUE] = luaL_checknumber(l, argi);
    }
    else
    {
        uniformValue.m4() = *CheckMatrix4FromLua(l, FIRST_VALUE);
    }

    SetModelUniform(model, uniformName, &uniformValue);
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
