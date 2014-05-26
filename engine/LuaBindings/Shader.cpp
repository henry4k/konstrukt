#include "Math.h"
#include "Shader.h"


const char* SHADER_TYPE = "Shader";
const char* SHADER_PROGRAM_TYPE = "ShaderProgram";


int Lua_Shader_destructor( lua_State* l )
{
    const Shader shader =
        *reinterpret_cast<Shader*>(lua_touserdata(l, 1));
    FreeShader(shader);
    return 0;
}

int Lua_ShaderProgram_destructor( lua_State* l )
{
    ShaderProgram* program =
        *reinterpret_cast<ShaderProgram**>(lua_touserdata(l, 1));
    FreeShaderProgram(program);
    return 0;
}

int Lua_LoadShader( lua_State* l )
{
    const char* fileName = luaL_checkstring(l, 1);
    const Shader shader = LoadShader(fileName);

    if(shader)
    {
        CopyUserDataToLua(l, SHADER_TYPE, sizeof(shader), &shader);
        return 1;
    }
    else
    {
        luaL_error(l, "Can't load shader '%s'", fileName);
        return 0;
    }
}

int Lua_LinkShaderProgram( lua_State* l )
{
    const int shaderCount = lua_gettop(l);
    Shader* shaders = new Shader[shaderCount];
    for(int i = 0; i < shaderCount; i++)
    {
        const Shader shader = GetShaderFromLua(l, i+1);
        if(shader != INVALID_SHADER)
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
        CopyUserDataToLua(l, SHADER_PROGRAM_TYPE, sizeof(program), &program);
        return 1;
    }
    else
    {
        luaL_error(l, "Can't create shader program.");
        return 0;
    }
}

int Lua_SetFloatUniform( lua_State* l )
{
    ShaderProgram* program = CheckShaderProgramFromLua(l, 1);
    const char* name = luaL_checkstring(l, 2);
    UniformValue value;
    value.f() = luaL_checknumber(l, 3);
    SetUniform(program, name, &value);
    return 0;
}

int Lua_SetVectorUniform( lua_State* l )
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

int Lua_SetMatrix4Uniform( lua_State* l )
{
    ShaderProgram* program = CheckShaderProgramFromLua(l, 1);
    const char* name = luaL_checkstring(l, 2);
    const glm::mat4* value = CheckMatrix4FromLua(l, 3);
    SetUniform(program, name, (const UniformValue*)value);
    return 0;
}

bool RegisterShaderInLua()
{
    if(!RegisterUserDataTypeInLua(SHADER_TYPE, Lua_Shader_destructor))
        return false;

    if(!RegisterUserDataTypeInLua(SHADER_PROGRAM_TYPE, Lua_ShaderProgram_destructor))
        return false;

    return
        RegisterFunctionInLua("LoadShader", Lua_LoadShader) &&
        RegisterFunctionInLua("LinkShaderProgram", Lua_LinkShaderProgram) &&
        RegisterFunctionInLua("SetFloatUniform", Lua_SetFloatUniform) &&
        RegisterFunctionInLua("SetVectorUniform", Lua_SetVectorUniform) &&
        RegisterFunctionInLua("SetMatrix4Uniform", Lua_SetMatrix4Uniform);
}

Shader GetShaderFromLua( lua_State* l, int stackPosition )
{
    const Shader* shaderPointer =
        (Shader*)GetUserDataFromLua(l, stackPosition, SHADER_TYPE);
    if(shaderPointer)
        return *shaderPointer;
    else
        return INVALID_SHADER;
}

Shader CheckShaderFromLua( lua_State* l, int stackPosition )
{
    return *(Shader*)CheckUserDataFromLua(l, stackPosition, SHADER_TYPE);
}

ShaderProgram* GetShaderProgramFromLua( lua_State* l, int stackPosition )
{
    return *(ShaderProgram**)GetUserDataFromLua(l, stackPosition, SHADER_PROGRAM_TYPE);
}

ShaderProgram* CheckShaderProgramFromLua( lua_State* l, int stackPosition )
{
    return *(ShaderProgram**)CheckUserDataFromLua(l, stackPosition, SHADER_PROGRAM_TYPE);
}
