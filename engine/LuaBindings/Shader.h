#ifndef __APOAPSIS_LUA_BINDINGS_SHADER__
#define __APOAPSIS_LUA_BINDINGS_SHADER__

#include "../Lua.h"
#include "../Shader.h"

bool RegisterShaderInLua();

ShaderObject GetShaderObjectFromLua( lua_State* l, int stackPosition );
ShaderObject CheckShaderObjectFromLua( lua_State* l, int stackPosition );
ShaderProgram GetShaderProgramFromLua( lua_State* l, int stackPosition );
ShaderProgram CheckShaderProgramFromLua( lua_State* l, int stackPosition );

#endif
