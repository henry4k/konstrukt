#ifndef __KONSTRUKT_LUA_BINDINGS_MODEL_WORLD__
#define __KONSTRUKT_LUA_BINDINGS_MODEL_WORLD__

struct lua_State;
struct ModelWorld;
struct Model;

ModelWorld* GetModelWorldFromLua( lua_State* l, int stackPosition );
ModelWorld* CheckModelWorldFromLua( lua_State* l, int stackPosition );

Model* GetModelFromLua( lua_State* l, int stackPosition );
Model* CheckModelFromLua( lua_State* l, int stackPosition );

bool RegisterModelWorldInLua();

#endif
