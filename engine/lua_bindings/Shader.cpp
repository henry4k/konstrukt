#include "../Lua.h"
#include "../Shader.h"
#include "Math.h"
#include "Texture.h"
#include "Shader.h"


// --- Shader ---

static int Lua_LoadShader( lua_State* l )
{
    const char* vfsPath = luaL_checkstring(l, 1);
    Shader* shader = LoadShader(vfsPath);

    if(shader)
    {
        PushPointerToLua(l, shader);
        ReferenceShader(shader);
        return 1;
    }
    else
    {
        return luaL_error(l, "Can't load shader '%s'", vfsPath);
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

static int Lua_GetShaderProgramShaderVariableSet( lua_State* l )
{
    ShaderProgram* program = CheckShaderProgramFromLua(l, 1);
    PushShaderVariableSetToLua(l, GetShaderProgramShaderVariableSet(program));
    return 1;
}

static int Lua_GetGlobalShaderVariableSet( lua_State* l )
{
    PushShaderVariableSetToLua(l, GetGlobalShaderVariableSet());
    return 1;
}

ShaderProgram* GetShaderProgramFromLua( lua_State* l, int stackPosition )
{
    return (ShaderProgram*)GetPointerFromLua(l, stackPosition);
}

ShaderProgram* CheckShaderProgramFromLua( lua_State* l, int stackPosition )
{
    return (ShaderProgram*)CheckPointerFromLua(l, stackPosition);
}


// --- ShaderProgramSet ---

static int Lua_CreateShaderProgramSet( lua_State* l )
{
    ShaderProgram* program = CheckShaderProgramFromLua(l, 1);
    ShaderProgramSet* set = CreateShaderProgramSet(program);

    if(set)
    {
        PushPointerToLua(l, set);
        ReferenceShaderProgramSet(set);
        return 1;
    }
    else
    {
        return luaL_error(l, "Can't create shader program set.");
    }
}

static int Lua_DestroyShaderProgramSet( lua_State* l )
{
    ShaderProgramSet* set = CheckShaderProgramSetFromLua(l, 1);
    ReleaseShaderProgramSet(set);
    return 0;
}

static int Lua_SetShaderProgramFamily( lua_State* l )
{
    ShaderProgramSet* set = CheckShaderProgramSetFromLua(l, 1);
    const char* family = luaL_checkstring(l, 2);
    ShaderProgram* program = CheckShaderProgramFromLua(l, 3);
    SetShaderProgramFamily(set, family, program);
    return 0;
}

ShaderProgramSet* GetShaderProgramSetFromLua( lua_State* l, int stackPosition )
{
    return (ShaderProgramSet*)GetPointerFromLua(l, stackPosition);
}

ShaderProgramSet* CheckShaderProgramSetFromLua( lua_State* l, int stackPosition )
{
    return (ShaderProgramSet*)CheckPointerFromLua(l, stackPosition);
}


// --- ShaderVariableSet ---

static int Lua_SetIntUniform( lua_State* l )
{
    ShaderVariableSet* set = CheckShaderVariableSetFromLua(l, 1);
    const char* name = luaL_checkstring(l, 2);
    const int value  = luaL_checkinteger(l, 3);
    SetIntUniform(set, name, value);
    return 0;
}

static int Lua_SetFloatUniform( lua_State* l )
{
    ShaderVariableSet* set = CheckShaderVariableSetFromLua(l, 1);
    const char* name  = luaL_checkstring(l, 2);
    const float value = luaL_checknumber(l, 3);
    SetFloatUniform(set, name, value);
    return 0;
}

static int Lua_SetVec3Uniform( lua_State* l )
{
    ShaderVariableSet* set = CheckShaderVariableSetFromLua(l, 1);
    const char* name = luaL_checkstring(l, 2);
    const Vec3 value = {{(float)luaL_checknumber(l, 3),
                         (float)luaL_checknumber(l, 4),
                         (float)luaL_checknumber(l, 5)}};
    SetVec3Uniform(set, name, value);
    return 0;
}

static int Lua_SetMat4Uniform( lua_State* l )
{
    ShaderVariableSet* set = CheckShaderVariableSetFromLua(l, 1);
    const char* name  = luaL_checkstring(l, 2);
    const Mat4* value = CheckMatrix4FromLua(l, 3);
    SetMat4Uniform(set, name, *value);
    return 0;
}

static int Lua_SetTexture( lua_State* l )
{
    ShaderVariableSet* set = CheckShaderVariableSetFromLua(l, 1);
    const char* name  = luaL_checkstring(l, 2);
    Texture* value = CheckTextureFromLua(l, 3);
    SetTexture(set, name, value);
    return 0;
}

static int Lua_UnsetShaderVariable( lua_State* l )
{
    ShaderVariableSet* set = CheckShaderVariableSetFromLua(l, 1);
    const char* name = luaL_checkstring(l, 2);
    UnsetShaderVariable(set, name);
    return 0;
}

static int Lua_ClearShaderVariableSet( lua_State* l )
{
    ShaderVariableSet* set = CheckShaderVariableSetFromLua(l, 1);
    ClearShaderVariableSet(set);
    return 0;
}

void PushShaderVariableSetToLua( lua_State* l, ShaderVariableSet* set )
{
    PushPointerToLua(l, set);
}

ShaderVariableSet* GetShaderVariableSetFromLua( lua_State* l, int stackPosition )
{
    return (ShaderVariableSet*)GetPointerFromLua(l, stackPosition);
}

ShaderVariableSet* CheckShaderVariableSetFromLua( lua_State* l, int stackPosition )
{
    return (ShaderVariableSet*)CheckPointerFromLua(l, stackPosition);
}


// --- Register in Lua ---

void RegisterShaderInLua()
{
    RegisterFunctionInLua("LoadShader", Lua_LoadShader);
    RegisterFunctionInLua("DestroyShader", Lua_DestroyShader);

    RegisterFunctionInLua("LinkShaderProgram", Lua_LinkShaderProgram);
    RegisterFunctionInLua("DestroyShaderProgram", Lua_DestroyShaderProgram);
    RegisterFunctionInLua("GetShaderProgramShaderVariableSet", Lua_GetShaderProgramShaderVariableSet);

    RegisterFunctionInLua("GetGlobalShaderVariableSet", Lua_GetGlobalShaderVariableSet);

    RegisterFunctionInLua("CreateShaderProgramSet", Lua_CreateShaderProgramSet);
    RegisterFunctionInLua("DestroyShaderProgramSet", Lua_DestroyShaderProgramSet);
    RegisterFunctionInLua("SetShaderProgramFamily", Lua_SetShaderProgramFamily);

    RegisterFunctionInLua("SetIntUniform", Lua_SetIntUniform);
    RegisterFunctionInLua("SetFloatUniform", Lua_SetFloatUniform);
    RegisterFunctionInLua("SetVec3Uniform", Lua_SetVec3Uniform);
    RegisterFunctionInLua("SetMat4Uniform", Lua_SetMat4Uniform);
    RegisterFunctionInLua("SetTexture", Lua_SetTexture);
    RegisterFunctionInLua("UnsetShaderVariable", Lua_UnsetShaderVariable);
    RegisterFunctionInLua("ClearShaderVariableSet", Lua_ClearShaderVariableSet);
}
