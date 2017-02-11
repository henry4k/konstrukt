#ifndef __KONSTRUKT_LUA_BINDINGS_CAMERA__
#define __KONSTRUKT_LUA_BINDINGS_CAMERA__

struct lua_State;
struct Camera;

Camera* GetCameraFromLua( lua_State* l, int stackPosition );
Camera* CheckCameraFromLua( lua_State* l, int stackPosition );

void RegisterCameraInLua();

#endif
