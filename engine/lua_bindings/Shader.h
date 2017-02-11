#ifndef __KONSTRUKT_LUA_BINDINGS_SHADER__
#define __KONSTRUKT_LUA_BINDINGS_SHADER__

struct lua_State;
struct Shader;
struct ShaderProgram;
struct ShaderProgramSet;
struct ShaderVariableSet;

Shader* GetShaderFromLua( lua_State* l, int stackPosition );
Shader* CheckShaderFromLua( lua_State* l, int stackPosition );

ShaderProgram* GetShaderProgramFromLua( lua_State* l, int stackPosition );
ShaderProgram* CheckShaderProgramFromLua( lua_State* l, int stackPosition );

ShaderProgramSet* GetShaderProgramSetFromLua( lua_State* l, int stackPosition );
ShaderProgramSet* CheckShaderProgramSetFromLua( lua_State* l, int stackPosition );

void PushShaderVariableSetToLua( lua_State* l, ShaderVariableSet* set );
ShaderVariableSet* GetShaderVariableSetFromLua( lua_State* l, int stackPosition );
ShaderVariableSet* CheckShaderVariableSetFromLua( lua_State* l, int stackPosition );

void RegisterShaderInLua();

#endif
