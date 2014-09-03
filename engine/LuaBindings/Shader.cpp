#include "../Lua.h"
#include "../Shader.h"
#include "Math.h"
#include "Shader.h"


// --- Shader ---

static int Lua_LoadShader( lua_State* l )
{
    const char* fileName = luaL_checkstring(l, 1);
    Shader* shader = LoadShader(fileName);

    if(shader)
    {
        PushPointerToLua(l, shader);
        ReferenceShader(shader);
        return 1;
    }
    else
    {
        return luaL_error(l, "Can't load shader '%s'", fileName);
    }
}

static int Lua_DestroyShader( lua_State* l )
{
    Shader* shader = CheckShaderFromLua(l, 1);
    ReleaseShader(shader);
    return 0;
}

Shader* GetShaderFromLua( lua_State* l, int stackPosition )
{
    return (Shader*)GetPointerFromLua(l, stackPosition);
}

Shader* CheckShaderFromLua( lua_State* l, int stackPosition )
{
    return (Shader*)CheckPointerFromLua(l, stackPosition);
}


// --- ShaderProgram ---

static int Lua_LinkShaderProgram( lua_State* l )
{
    const int shaderCount = lua_gettop(l);
    Shader** shaders = new Shader*[shaderCount];
    for(int i = 0; i < shaderCount; i++)
    {
        Shader* shader = GetShaderFromLua(l, i+1);
        if(shader)
        {
            shaders[i] = shader;
        }
        else
        {
            delete[] shaders;
            return luaL_argerror(l, i+1, "Can't retrieve shader.");
        }
    }

    ShaderProgram* program = LinkShaderProgram(shaders, shaderCount);
    delete[] shaders;
    if(program)
    {
        PushPointerToLua(l, program);
        ReferenceShaderProgram(program);
        return 1;
    }
    else
    {
        return luaL_error(l, "Can't create shader program.");
    }
}

static int Lua_DestroyShaderProgram( lua_State* l )
{
    ShaderProgram* program = CheckShaderProgramFromLua(l, 1);
    ReleaseShaderProgram(program);
    return 0;
}

static int Lua_SetGlobalUniform( lua_State* l )
{
    const char* name = luaL_checkstring(l, 1);

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
    const UniformType type = (UniformType)luaL_checkoption(l, 2, NULL, types);

    UniformValue value;
    switch(type)
    {
        case INT_UNIFORM:
            value.i = (int)luaL_checknumber(l, 3);
            SetGlobalUniform(name, type, &value);
            break;

        case FLOAT_UNIFORM:
            value.f = luaL_checknumber(l, 3);
            SetGlobalUniform(name, type, &value);
            break;

        case VEC3_UNIFORM:
            value.data[0] = luaL_checknumber(l, 3);
            value.data[1] = luaL_checknumber(l, 4);
            value.data[2] = luaL_checknumber(l, 5);
            SetGlobalUniform(name, type, &value);
            break;

        case VEC4_UNIFORM:
            value.data[0] = luaL_checknumber(l, 3);
            value.data[1] = luaL_checknumber(l, 4);
            value.data[2] = luaL_checknumber(l, 5);
            value.data[3] = luaL_checknumber(l, 6);
            SetGlobalUniform(name, type, &value);
            break;

        case MAT3_UNIFORM:
            return luaL_argerror(l, 2, "mat3 is not support by the Lua api.");

        case MAT4_UNIFORM:
            const glm::mat4* m = CheckMatrix4FromLua(l, 3);
            SetGlobalUniform(name, type, (const UniformValue*)&m);
            break;
    }

    return 0;
}

static int Lua_UnsetGlobalUniform( lua_State* l )
{
    const char* name = luaL_checkstring(l, 1);
    UnsetGlobalUniform(name);
    return 0;
}

ShaderProgram* GetShaderProgramFromLua( lua_State* l, int stackPosition )
{
    return (ShaderProgram*)GetPointerFromLua(l, stackPosition);
}

ShaderProgram* CheckShaderProgramFromLua( lua_State* l, int stackPosition )
{
    return (ShaderProgram*)CheckPointerFromLua(l, stackPosition);
}


// --- Register in Lua ---

bool RegisterShaderInLua()
{
    return
        RegisterFunctionInLua("LoadShader", Lua_LoadShader) &&
        RegisterFunctionInLua("DestroyShader", Lua_DestroyShader) &&

        RegisterFunctionInLua("LinkShaderProgram", Lua_LinkShaderProgram) &&
        RegisterFunctionInLua("DestroyShaderProgram", Lua_DestroyShaderProgram) &&

        RegisterFunctionInLua("SetGlobalUniform", Lua_SetGlobalUniform) &&
        RegisterFunctionInLua("UnsetGlobalUniform", Lua_UnsetGlobalUniform);
}
