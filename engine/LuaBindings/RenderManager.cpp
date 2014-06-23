#include <string.h> // memset

#include "../Lua.h"
#include "../Mesh.h"
#include "../RenderManager.h"
#include "Math.h"
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include "RenderManager.h"


static const char* MODEL_TYPE = "Model";

static int Lua_Model_destructor( lua_State* l )
{
    Model* model = CheckModelFromLua(l, 1);
    ReleaseModel(model);
    return 0;
}

static int Lua_CreateModel( lua_State* l )
{
    ShaderProgram* program = CheckShaderProgramFromLua(l, 1);

    Model* model = CreateModel(program);
    if(model &&
       CopyUserDataToLua(l, MODEL_TYPE, sizeof(model), &model))
    {
        ReferenceModel(model);
        return 1;
    }
    else
    {
        lua_pop(l, 1);
        luaL_error(l, "Can't create more models.");
        return 0;
    }
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
    Texture* texture = CheckTextureFromLua(l, 2);
    SetModelTexture(model, texture);
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

bool RegisterRenderManagerInLua()
{
    if(!RegisterUserDataTypeInLua(MODEL_TYPE, Lua_Model_destructor))
        return false;

    return
        RegisterFunctionInLua("CreateModel", Lua_CreateModel) &&
        RegisterFunctionInLua("SetModelTransformation", Lua_SetModelTransformation) &&
        RegisterFunctionInLua("SetModelMesh", Lua_SetModelMesh) &&
        RegisterFunctionInLua("SetModelTexture", Lua_SetModelTexture) &&
        RegisterFunctionInLua("SetModelUniform", Lua_SetModelUniform) &&
        RegisterFunctionInLua("UnsetModelUniform", Lua_UnsetModelUniform);
}

Model* GetModelFromLua( lua_State* l, int stackPosition )
{
    return *(Model**)GetUserDataFromLua(l, stackPosition, MODEL_TYPE);
}

Model* CheckModelFromLua( lua_State* l, int stackPosition )
{
    return *(Model**)CheckUserDataFromLua(l, stackPosition, MODEL_TYPE);
}
