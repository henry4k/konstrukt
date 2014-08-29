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
        luaL_error(l, "Can't load shader '%s'", fileName);
        return 0;
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
            luaL_argerror(l, i+1, "Can't retrieve shader.");
            return 0;
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
        luaL_error(l, "Can't create shader program.");
        return 0;
    }
}

static int Lua_DestroyShaderProgram( lua_State* l )
{
    ShaderProgram* program = CheckShaderProgramFromLua(l, 1);
    ReleaseShaderProgram(program);
    return 0;
}

static int Lua_SetFloatUniform( lua_State* l )
{
    ShaderProgram* program = CheckShaderProgramFromLua(l, 1);
    const char* name = luaL_checkstring(l, 2);
    UniformValue value;
    value.f() = luaL_checknumber(l, 3);
    SetUniform(program, name, &value);
    return 0;
}

static int Lua_SetVectorUniform( lua_State* l )
{
    ShaderProgram* program = CheckShaderProgramFromLua(l, 1);
    const char* name = luaL_checkstring(l, 2);

    const int valueCount = lua_gettop(l)-2;
    if(valueCount <= 0)
        luaL_error(l, "Too few arguments!");
    if(valueCount > 4)
        luaL_error(l, "Too many arguments!");
    UniformValue uniformValue;

    for(int i = 0; i < valueCount; i++)
    {
        const int stackPosition = i+2+1;
        if(lua_type(l, stackPosition) == LUA_TNUMBER)
        {
            uniformValue.data[i] = lua_tonumber(l, stackPosition);
        }
        else
        {
            luaL_argerror(l, stackPosition, "Not a number");
            return 0;
        }
    }

    SetUniform(program, name, &uniformValue);
    return 0;
}

static int Lua_SetMatrix4Uniform( lua_State* l )
{
    ShaderProgram* program = CheckShaderProgramFromLua(l, 1);
    const char* name = luaL_checkstring(l, 2);
    const glm::mat4* value = CheckMatrix4FromLua(l, 3);
    SetUniform(program, name, (const UniformValue*)value);
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
        RegisterFunctionInLua("SetFloatUniform", Lua_SetFloatUniform) &&
        RegisterFunctionInLua("SetVectorUniform", Lua_SetVectorUniform) &&
        RegisterFunctionInLua("SetMatrix4Uniform", Lua_SetMatrix4Uniform);
}
