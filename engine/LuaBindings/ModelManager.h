#ifndef __APOAPSIS_LUA_BINDINGS_MODEL_MANAGER__
#define __APOAPSIS_LUA_BINDINGS_MODEL_MANAGER__

struct lua_State;
struct Model;

Model* GetModelFromLua( lua_State* l, int stackPosition );
Model* CheckModelFromLua( lua_State* l, int stackPosition );

bool RegisterModelManagerInLua();

#endif
