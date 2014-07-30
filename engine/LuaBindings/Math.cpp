#include "Math.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/projection.hpp>

using namespace glm;


// --- Quaternion ---

const char* QUATERNION_TYPE = "Quaternion";

int Lua_CreateQuaternion( lua_State* l )
{
    const quat r;
    CopyUserDataToLua(l, QUATERNION_TYPE, sizeof(quat), &r);
    return 1;
}

int Lua_CreateQuaternionFromEulerAngles( lua_State* l )
{
    const quat r(vec3(luaL_checknumber(l, 1),
                      luaL_checknumber(l, 2),
                      luaL_checknumber(l, 3)));
    CopyUserDataToLua(l, QUATERNION_TYPE, sizeof(quat), &r);
    return 1;
}

int Lua_CreateQuaternionFromMatrix( lua_State* l )
{
    const mat4* matrix = CheckMatrix4FromLua(l, 1);
    const quat r(*matrix);
    CopyUserDataToLua(l, QUATERNION_TYPE, sizeof(quat), &r);
    return 1;
}

int Lua_CopyQuaternion( lua_State* l )
{
    const quat* source = CheckQuaternionFromLua(l, 1);
    CopyUserDataToLua(l, QUATERNION_TYPE, sizeof(quat), source);
    return 1;
}

int Lua_InvertQuaternion( lua_State* l )
{
    const quat* source = CheckQuaternionFromLua(l, 1);
    quat* r = (quat*)PushUserDataToLua(l, QUATERNION_TYPE, sizeof(quat));
    *r = inverse(*source);
    return 1;
}

int Lua_QuaternionOp( lua_State* l )
{
    const quat* a = CheckQuaternionFromLua(l, 1);
    const quat* b = CheckQuaternionFromLua(l, 2);
    const char* op = luaL_checkstring(l, 3);
    quat* r = (quat*)PushUserDataToLua(l, QUATERNION_TYPE, sizeof(quat));
    switch(*op)
    {
        case '+': *r = *a + *b; break;
        case '*': *r = *a * *b; break;
        default: return luaL_error(l, "Unknown operation '%s'", op);
    }
    return 1;
}

int Lua_LerpQuaternion( lua_State* l )
{
    const quat* a = CheckQuaternionFromLua(l, 1);
    const quat* b = CheckQuaternionFromLua(l, 2);
    const float f = luaL_checknumber(l, 3);
    const char* mode = luaL_checkstring(l, 4);
    quat* r = (quat*)PushUserDataToLua(l, QUATERNION_TYPE, sizeof(quat));
    switch(*mode)
    {
        case 'l': *r = lerp(*a, *b, f); break;
        case 's': *r = slerp(*a, *b, f); break;
        default: return luaL_error(l, "Unknown mode '%s'", mode);
    }
    return 1;
}

quat* CreateQuaternionInLua( lua_State* l )
{
    return (quat*)PushUserDataToLua(l, QUATERNION_TYPE, sizeof(quat));
}

quat* GetQuaternionFromLua( lua_State* l, int stackPosition )
{
    return (quat*)GetUserDataFromLua(l, stackPosition, QUATERNION_TYPE);
}

quat* CheckQuaternionFromLua( lua_State* l, int stackPosition )
{
    return (quat*)CheckUserDataFromLua(l, stackPosition, QUATERNION_TYPE);
}


// --- Matrix4 ---

const char* MATRIX4_TYPE = "Matrix4";

int Lua_CreateMatrix4( lua_State* l )
{
    const mat4 matrix(1.0f);
    CopyUserDataToLua(l, MATRIX4_TYPE, sizeof(mat4), &matrix);
    return 1;
}

int Lua_CopyMatrix4( lua_State* l )
{
    const mat4* source = CheckMatrix4FromLua(l, 1);
    CopyUserDataToLua(l, MATRIX4_TYPE, sizeof(mat4), source);
    return 1;
}

int Lua_Matrix4Op( lua_State* l )
{
    const mat4* a = CheckMatrix4FromLua(l, 1);
    const mat4* b = CheckMatrix4FromLua(l, 2);
    const char* op = luaL_checkstring(l, 3);

    mat4* r = (mat4*)PushUserDataToLua(l, MATRIX4_TYPE, sizeof(mat4));
    switch(*op)
    {
        case '+': *r = *a + *b; break;
        case '-': *r = *a - *b; break;
        case '*': *r = *a * *b; break;
        case '/': *r = *a / *b; break;
        default: return luaL_error(l, "Unknown operation '%s'", op);
    }
    return 1;
}

int Lua_TranslateMatrix4( lua_State* l )
{
    const mat4* a = CheckMatrix4FromLua(l, 1);

    const vec3 t(
        luaL_checknumber(l, 2),
        luaL_checknumber(l, 3),
        luaL_checknumber(l, 4)
    );

    mat4* destination = (mat4*)PushUserDataToLua(l, MATRIX4_TYPE, sizeof(mat4));
    *destination = translate(*a, t);
    return 1;
}

int Lua_ScaleMatrix4( lua_State* l )
{
    const mat4* a = CheckMatrix4FromLua(l, 1);

    const vec3 s(
        luaL_checknumber(l, 2),
        luaL_checknumber(l, 3),
        luaL_checknumber(l, 4)
    );

    mat4* destination = (mat4*)PushUserDataToLua(l, MATRIX4_TYPE, sizeof(mat4));
    *destination = scale(*a, s);
    return 1;
}

int Lua_RotateMatrix4( lua_State* l )
{
    const mat4* a = CheckMatrix4FromLua(l, 1);

    const float angle = luaL_checknumber(l, 2);

    const vec3 v(
        luaL_checknumber(l, 3),
        luaL_checknumber(l, 4),
        luaL_checknumber(l, 5)
    );

    mat4* destination = (mat4*)PushUserDataToLua(l, MATRIX4_TYPE, sizeof(mat4));
    *destination = rotate(*a, angle, v);
    return 1;
}

int Lua_Matrix4TransformVector( lua_State* l )
{
    const mat4* a = CheckMatrix4FromLua(l, 1);

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
    const mat4* a = CheckMatrix4FromLua(l, 1);

    mat4* destination = (mat4*)PushUserDataToLua(l, MATRIX4_TYPE, sizeof(mat4));
    *destination = MakeRotationMatrix(*a);
    return 1;
}

mat4* CreateMatrix4InLua( lua_State* l )
{
    return (mat4*)PushUserDataToLua(l, MATRIX4_TYPE, sizeof(mat4));
}

mat4* GetMatrix4FromLua( lua_State* l, int stackPosition )
{
    return (mat4*)GetUserDataFromLua(l, stackPosition, MATRIX4_TYPE);
}

mat4* CheckMatrix4FromLua( lua_State* l, int stackPosition )
{
    return (mat4*)CheckUserDataFromLua(l, stackPosition, MATRIX4_TYPE);
}


// --- Register in Lua ---

bool RegisterMathInLua()
{
    return
        RegisterUserDataTypeInLua(QUATERNION_TYPE, NULL) &&
        RegisterFunctionInLua("CreateQuaternion", Lua_CreateQuaternion) &&
        RegisterFunctionInLua("CreateQuaternionFromEulerAngles", Lua_CreateQuaternionFromEulerAngles) &&
        RegisterFunctionInLua("CreateQuaternionFromMatrix", Lua_CreateQuaternionFromMatrix) &&
        RegisterFunctionInLua("CopyQuaternion", Lua_CopyQuaternion) &&
        RegisterFunctionInLua("InvertQuaternion", Lua_InvertQuaternion) &&
        RegisterFunctionInLua("QuaternionOp", Lua_QuaternionOp) &&
        RegisterFunctionInLua("LerpQuaternion", Lua_LerpQuaternion) &&

        RegisterUserDataTypeInLua(MATRIX4_TYPE, NULL) &&
        RegisterFunctionInLua("CreateMatrix4", Lua_CreateMatrix4) &&
        RegisterFunctionInLua("CopyMatrix4", Lua_CopyMatrix4) &&
        RegisterFunctionInLua("Matrix4Op", Lua_Matrix4Op) &&
        RegisterFunctionInLua("TranslateMatrix4", Lua_TranslateMatrix4) &&
        RegisterFunctionInLua("ScaleMatrix4", Lua_ScaleMatrix4) &&
        RegisterFunctionInLua("RotateMatrix4", Lua_RotateMatrix4) &&
        RegisterFunctionInLua("Matrix4TransformVector", Lua_Matrix4TransformVector) &&
        RegisterFunctionInLua("MakeRotationMatrix", Lua_MakeRotationMatrix);
}
