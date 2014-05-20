#include "../Lua.h"
#include "../Math.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/projection.hpp>

using namespace glm;


int Lua_CreateMatrix4( lua_State* l )
{
    const mat4 matrix(1.0f);
    mat4* destination = (mat4*)lua_newuserdata(l, sizeof(matrix));
    *destination = matrix;
    return 1;
}

int Lua_CopyMatrix4( lua_State* l )
{
    luaL_checktype(l, 1, LUA_TUSERDATA);
    const mat4* source = (mat4*)lua_touserdata(l, 1);

    mat4* destination = (mat4*)lua_newuserdata(l, sizeof(mat4));
    *destination = *source;
    return 1;
}

int Lua_AddMatrix4( lua_State* l )
{
    luaL_checktype(l, 1, LUA_TUSERDATA);
    const mat4* a = (mat4*)lua_touserdata(l, 1);

    luaL_checktype(l, 2, LUA_TUSERDATA);
    const mat4* b = (mat4*)lua_touserdata(l, 2);

    mat4* destination = (mat4*)lua_newuserdata(l, sizeof(mat4));
    *destination = *a + *b;
    return 1;
}

int Lua_SubMatrix4( lua_State* l )
{
    luaL_checktype(l, 1, LUA_TUSERDATA);
    const mat4* a = (mat4*)lua_touserdata(l, 1);

    luaL_checktype(l, 2, LUA_TUSERDATA);
    const mat4* b = (mat4*)lua_touserdata(l, 2);

    mat4* destination = (mat4*)lua_newuserdata(l, sizeof(mat4));
    *destination = *a - *b;
    return 1;
}

int Lua_MulMatrix4( lua_State* l )
{
    luaL_checktype(l, 1, LUA_TUSERDATA);
    const mat4* a = (mat4*)lua_touserdata(l, 1);

    luaL_checktype(l, 2, LUA_TUSERDATA);
    const mat4* b = (mat4*)lua_touserdata(l, 2);

    mat4* destination = (mat4*)lua_newuserdata(l, sizeof(mat4));
    *destination = *a * *b;
    return 1;
}

int Lua_DivMatrix4( lua_State* l )
{
    luaL_checktype(l, 1, LUA_TUSERDATA);
    const mat4* a = (mat4*)lua_touserdata(l, 1);

    luaL_checktype(l, 2, LUA_TUSERDATA);
    const mat4* b = (mat4*)lua_touserdata(l, 2);

    mat4* destination = (mat4*)lua_newuserdata(l, sizeof(mat4));
    *destination = *a / *b;
    return 1;
}

int Lua_TranslateMatrix4( lua_State* l )
{
    luaL_checktype(l, 1, LUA_TUSERDATA);
    const mat4* a = (mat4*)lua_touserdata(l, 1);

    const vec3 t(
        luaL_checknumber(l, 2),
        luaL_checknumber(l, 3),
        luaL_checknumber(l, 4)
    );

    mat4* destination = (mat4*)lua_newuserdata(l, sizeof(mat4));
    *destination = translate(*a, t);
    return 1;
}

int Lua_ScaleMatrix4( lua_State* l )
{
    luaL_checktype(l, 1, LUA_TUSERDATA);
    const mat4* a = (mat4*)lua_touserdata(l, 1);

    const vec3 s(
        luaL_checknumber(l, 2),
        luaL_checknumber(l, 3),
        luaL_checknumber(l, 4)
    );

    mat4* destination = (mat4*)lua_newuserdata(l, sizeof(mat4));
    *destination = scale(*a, s);
    return 1;
}

int Lua_RotateMatrix4( lua_State* l )
{
    luaL_checktype(l, 1, LUA_TUSERDATA);
    const mat4* a = (mat4*)lua_touserdata(l, 1);

    const float angle = luaL_checknumber(l, 2);

    const vec3 v(
        luaL_checknumber(l, 3),
        luaL_checknumber(l, 4),
        luaL_checknumber(l, 5)
    );

    mat4* destination = (mat4*)lua_newuserdata(l, sizeof(mat4));
    *destination = rotate(*a, angle, v);
    return 1;
}

int Lua_Matrix4TransformVector( lua_State* l )
{
    luaL_checktype(l, 1, LUA_TUSERDATA);
    const mat4* a = (mat4*)lua_touserdata(l, 1);

    const vec4 v(
        luaL_checknumber(l, 2),
        luaL_checknumber(l, 3),
        luaL_checknumber(l, 4),
        luaL_checknumber(l, 5)
    );

    const vec4 r = *a * v;
    lua_pushnumber(l, r[0]);
    lua_pushnumber(l, r[1]);
    lua_pushnumber(l, r[2]);
    lua_pushnumber(l, r[3]);
    return 4;
}

int Lua_MakeRotationMatrix( lua_State* l )
{
    luaL_checktype(l, 1, LUA_TUSERDATA);
    const mat4* a = (mat4*)lua_touserdata(l, 1);

    mat4* destination = (mat4*)lua_newuserdata(l, sizeof(mat4));
    *destination = MakeRotationMatrix(*a);
    return 1;
}

bool RegisterMathInLua()
{
    return
        RegisterFunctionInLua("CreateMatrix4", Lua_CreateMatrix4) &&
        RegisterFunctionInLua("CopyMatrix4", Lua_CopyMatrix4) &&
        RegisterFunctionInLua("AddMatrix4", Lua_AddMatrix4) &&
        RegisterFunctionInLua("SubMatrix4", Lua_SubMatrix4) &&
        RegisterFunctionInLua("MulMatrix4", Lua_MulMatrix4) &&
        RegisterFunctionInLua("DivMatrix4", Lua_DivMatrix4) &&
        RegisterFunctionInLua("TranslateMatrix4", Lua_TranslateMatrix4) &&
        RegisterFunctionInLua("ScaleMatrix4", Lua_ScaleMatrix4) &&
        RegisterFunctionInLua("RotateMatrix4", Lua_RotateMatrix4) &&
        RegisterFunctionInLua("Matrix4TransformVector", Lua_Matrix4TransformVector) &&
        RegisterFunctionInLua("MakeRotationMatrix", Lua_MakeRotationMatrix);
}
