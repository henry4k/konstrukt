#include "Shader.h"


const char* SHADER_OBJECT_TYPE = "ShaderObject";
const char* SHADER_PROGRAM_TYPE = "ShaderProgram";


int Lua_ShaderObject_destructor( lua_State* l )
{
    const ShaderObject object =
        *reinterpret_cast<ShaderObject*>(lua_touserdata(l, 1));
    FreeShaderObject(object);
    return 0;
}

int Lua_ShaderProgram_destructor( lua_State* l )
{
    const ShaderProgram program =
        *reinterpret_cast<ShaderProgram*>(lua_touserdata(l, 1));
    FreeShaderProgram(program);
    return 0;
}

int Lua_LoadShaderObject( lua_State* l )
{
    const char* fileName = luaL_checkstring(l, 1);
    const ShaderObject object = LoadShaderObject(fileName);

    if(object && CopyUserDataToLua(l, SHADER_OBJECT_TYPE, sizeof(object), &object))
    {
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
    ShaderObject* objects = new ShaderObject[objectCount];
    for(int i = 0; i < objectCount; i++)
    {
        const ShaderObject object = GetShaderObjectFromLua(l, i+1);
        if(object != INVALID_SHADER_OBJECT)
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
    if(program != INVALID_SHADER_PROGRAM &&
       CopyUserDataToLua(l, SHADER_PROGRAM_TYPE, sizeof(program), &program))
    {
        return 1;
    }
    else
    {
        luaL_error(l, "Can't create shader program.");
        return 0;
    }
}

bool RegisterShaderInLua()
{
    if(!RegisterUserDataTypeInLua(SHADER_OBJECT_TYPE, Lua_ShaderObject_destructor))
        return false;

    if(!RegisterUserDataTypeInLua(SHADER_PROGRAM_TYPE, Lua_ShaderProgram_destructor))
        return false;

    return
        RegisterFunctionInLua("LoadShaderObject", Lua_LoadShaderObject) &&
        RegisterFunctionInLua("LinkShaderProgram", Lua_LinkShaderProgram); // TODO
}

ShaderObject GetShaderObjectFromLua( lua_State* l, int stackPosition )
{
    return *(ShaderObject*)GetUserDataFromLua(l, stackPosition, SHADER_OBJECT_TYPE);
}

ShaderObject CheckShaderObjectFromLua( lua_State* l, int stackPosition )
{
    return *(ShaderObject*)CheckUserDataFromLua(l, stackPosition, SHADER_OBJECT_TYPE);
}

ShaderProgram GetShaderProgramFromLua( lua_State* l, int stackPosition )
{
    return *(ShaderProgram*)GetUserDataFromLua(l, stackPosition, SHADER_PROGRAM_TYPE);
}

ShaderProgram CheckShaderProgramFromLua( lua_State* l, int stackPosition )
{
    return *(ShaderProgram*)CheckUserDataFromLua(l, stackPosition, SHADER_PROGRAM_TYPE);
}
