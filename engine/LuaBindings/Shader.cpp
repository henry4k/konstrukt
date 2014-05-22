#include "Math.h"
#include "Shader.h"


const char* SHADER_TYPE = "Shader";
const char* SHADER_PROGRAM_TYPE = "ShaderProgram";


int Lua_Shader_destructor( lua_State* l )
{
    const Shader object =
        *reinterpret_cast<Shader*>(lua_touserdata(l, 1));
    FreeShader(object);
    return 0;
}

int Lua_ShaderProgram_destructor( lua_State* l )
{
    const ShaderProgram program =
        *reinterpret_cast<ShaderProgram*>(lua_touserdata(l, 1));
    FreeShaderProgram(program);
    return 0;
}

int Lua_LoadShader( lua_State* l )
{
    const char* fileName = luaL_checkstring(l, 1);
    const Shader object = LoadShader(fileName);

    if(object)
    {
        CopyUserDataToLua(l, SHADER_TYPE, sizeof(object), &object);
        return 1;
    }
    else
    {
        luaL_error(l, "Can't load shader object '%s'", fileName);
        return 0;
    }
}

int Lua_LinkShaderProgram( lua_State* l )
{
    const int objectCount = lua_gettop(l);
    Shader* objects = new Shader[objectCount];
    for(int i = 0; i < objectCount; i++)
    {
        const Shader object = GetShaderFromLua(l, i+1);
        if(object != INVALID_SHADER)
        {
            objects[i] = object;
        }
        else
        {
            delete[] objects;
            luaL_argerror(l, i+1, "Can't retrieve shader object.");
            return 0;
        }
    }

    const ShaderProgram program = LinkShaderProgram(objects, objectCount);
    delete[] objects;
    if(program != INVALID_SHADER_PROGRAM)
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
    const ShaderProgram program = CheckShaderProgramFromLua(l, 1);
    const char* name = luaL_checkstring(l, 2);
    const float value = luaL_checknumber(l, 3);
    SetUniform(program, name, value);
    return 0;
}

int Lua_SetVectorUniform( lua_State* l )
{
    const ShaderProgram program = CheckShaderProgramFromLua(l, 1);
    const char* name = luaL_checkstring(l, 2);

    const int valueCount = lua_gettop(l)-2;
    if(valueCount <= 0)
        luaL_error(l, "Too few arguments!");
    float* values = new float[valueCount];

    for(int i = 0; i < valueCount; i++)
    {
        const int stackPosition = i+2+1;
        if(lua_type(l, stackPosition) == LUA_TNUMBER)
        {
            values[i] = lua_tonumber(l, stackPosition);
        }
        else
        {
            delete[] values;
            luaL_argerror(l, stackPosition, "Not a number");
            return 0;
        }
    }

    SetUniform(program, name, valueCount, values);
    delete[] values;
    return 0;
}

int Lua_SetMatrix4Uniform( lua_State* l )
{
    const ShaderProgram program = CheckShaderProgramFromLua(l, 1);
    const char* name = luaL_checkstring(l, 2);
    const glm::mat4* value = CheckMatrix4FromLua(l, 3);
    SetUniformMatrix4(program, name, value);
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
    const Shader* objectPointer =
        (Shader*)GetUserDataFromLua(l, stackPosition, SHADER_TYPE);
    if(objectPointer)
        return *objectPointer;
    else
        return INVALID_SHADER;
}

Shader CheckShaderFromLua( lua_State* l, int stackPosition )
{
    return *(Shader*)CheckUserDataFromLua(l, stackPosition, SHADER_TYPE);
}

ShaderProgram GetShaderProgramFromLua( lua_State* l, int stackPosition )
{
    const ShaderProgram* programPointer =
        (ShaderProgram*)GetUserDataFromLua(l, stackPosition, SHADER_PROGRAM_TYPE);
    if(programPointer)
        return *programPointer;
    else
        return INVALID_SHADER_PROGRAM;
}

ShaderProgram CheckShaderProgramFromLua( lua_State* l, int stackPosition )
{
    return *(ShaderProgram*)CheckUserDataFromLua(l, stackPosition, SHADER_PROGRAM_TYPE);
}
