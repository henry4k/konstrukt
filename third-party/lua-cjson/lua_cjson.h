#ifndef __LUA_CJSON_H__
#define __LUA_CJSON_H__

#include <lua.h>

int luaopen_cjson(lua_State *l);
int luaopen_cjson_safe(lua_State *l);

#endif
