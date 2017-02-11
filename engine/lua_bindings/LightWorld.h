#ifndef __KONSTRUKT_LUA_BINDINGS_LIGHT_WORLD__
#define __KONSTRUKT_LUA_BINDINGS_LIGHT_WORLD__

struct lua_State;
struct LightWorld;
struct Light;

LightWorld* GetLightWorldFromLua( lua_State* l, int stackPosition );
LightWorld* CheckLightWorldFromLua( lua_State* l, int stackPosition );

Light* GetLightFromLua( lua_State* l, int stackPosition );
Light* CheckLightFromLua( lua_State* l, int stackPosition );

void RegisterLightWorldInLua();

#endif
