#ifndef __APOAPSIS_LUA_BINDINGS_SHADER__
#define __APOAPSIS_LUA_BINDINGS_SHADER__

#include "../Lua.h"
#include "../Shader.h"

bool RegisterShaderInLua();

Shader GetShaderFromLua( lua_State* l, int stackPosition );
Shader CheckShaderFromLua( lua_State* l, int stackPosition );
ShaderProgram* GetShaderProgramFromLua( lua_State* l, int stackPosition );
ShaderProgram* CheckShaderProgramFromLua( lua_State* l, int stackPosition );

#endif
