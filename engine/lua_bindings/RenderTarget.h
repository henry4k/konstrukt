#ifndef __KONSTRUKT_LUA_BINDINGS_RENDER_TARGET__
#define __KONSTRUKT_LUA_BINDINGS_RENDER_TARGET__

struct lua_State;
struct RenderTarget;

RenderTarget* GetRenderTargetFromLua( lua_State* l, int stackPosition );
RenderTarget* CheckRenderTargetFromLua( lua_State* l, int stackPosition );

void RegisterRenderTargetInLua();

#endif
