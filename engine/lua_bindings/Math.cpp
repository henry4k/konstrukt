#include "../Common.h"
#include "../Lua.h"
#include "Math.h"


// To suppress this C++11 warning:
// narrowing conversion of ‘luaL_checknumber(l, 2)’ from
// ‘lua_Number {aka double}’ to ‘float’ inside { } is ill-formed in C++11
static Vec3 CreateVec3( float x, float y, float z )
{
    Vec3 r = {{x,y,z}};
    return r;
}


// --- Quaternion ---

static const char* QUATERNION_TYPE = "Quaternion";

static int Lua_CreateQuaternion( lua_State* l )
{
    CopyUserDataToLua(l, QUATERNION_TYPE, sizeof(Quat), &QuatIdentity);
    return 1;
}

static int Lua_CreateQuaternionByAngleAndAxis( lua_State* l )
{
    const float angle = luaL_checknumber(l, 1);
    const Vec3  axis = CreateVec3(luaL_checknumber(l, 2),
                                  luaL_checknumber(l, 3),
                                  luaL_checknumber(l, 4));
    const Quat r = QuatFromAngleAndAxis(angle, axis);
    CopyUserDataToLua(l, QUATERNION_TYPE, sizeof(Quat), &r);
    return 1;
}

static int Lua_CopyQuaternion( lua_State* l )
{
    const Quat* source = CheckQuaternionFromLua(l, 1);
    CopyUserDataToLua(l, QUATERNION_TYPE, sizeof(Quat), source);
    return 1;
}

static int Lua_NormalizeQuaternion( lua_State* l )
{
    const Quat* source = CheckQuaternionFromLua(l, 1);
    Quat* r = (Quat*)PushUserDataToLua(l, QUATERNION_TYPE, sizeof(Quat));
    *r = NormalizeQuat(*source);
    return 1;
}

static int Lua_QuaternionConjugate( lua_State* l )
{
    const Quat* source = CheckQuaternionFromLua(l, 1);
    Quat* r = (Quat*)PushUserDataToLua(l, QUATERNION_TYPE, sizeof(Quat));
    *r = QuatConjugate(*source);
    return 1;
}

static int Lua_InvertQuaternion( lua_State* l )
{
    const Quat* source = CheckQuaternionFromLua(l, 1);
    Quat* r = (Quat*)PushUserDataToLua(l, QUATERNION_TYPE, sizeof(Quat));
    *r = InvertQuat(*source);
    return 1;
}

static int Lua_MultiplyQuaternion( lua_State* l )
{
    const Quat* a = CheckQuaternionFromLua(l, 1);
    const Quat* b = CheckQuaternionFromLua(l, 2);
    Quat* r = (Quat*)PushUserDataToLua(l, QUATERNION_TYPE, sizeof(Quat));
    *r = MulQuat(*a, *b);
    return 1;
}

static int Lua_QuaternionXVector3( lua_State* l )
{
    const Quat* a = CheckQuaternionFromLua(l, 1);
    const Vec3 b = CreateVec3(luaL_checknumber(l, 2),
                              luaL_checknumber(l, 3),
                              luaL_checknumber(l, 4));
    const Vec3 r = MulQuatByVec3(*a, b);
    lua_pushnumber(l, r._[0]);
    lua_pushnumber(l, r._[1]);
    lua_pushnumber(l, r._[2]);
    return 3;
}

Quat* CreateQuaternionInLua( lua_State* l )
{
    return (Quat*)PushUserDataToLua(l, QUATERNION_TYPE, sizeof(Quat));
}

Quat* GetQuaternionFromLua( lua_State* l, int stackPosition )
{
    return (Quat*)GetUserDataFromLua(l, stackPosition, QUATERNION_TYPE);
}

Quat* CheckQuaternionFromLua( lua_State* l, int stackPosition )
{
    return (Quat*)CheckUserDataFromLua(l, stackPosition, QUATERNION_TYPE);
}


// --- Matrix4 ---

static const char* MATRIX_TYPE = "Matrix4";

static int Lua_CreateMatrix4( lua_State* l )
{
    CopyUserDataToLua(l, MATRIX_TYPE, sizeof(Mat4), &Mat4Identity);
    return 1;
}

static int Lua_CreateMatrix4FromQuaternion( lua_State* l )
{
    const Quat* s = CheckQuaternionFromLua(l, 1);
    Mat4* r = (Mat4*)PushUserDataToLua(l, MATRIX_TYPE, sizeof(Mat4));
    *r = Mat4FromQuat(*s);
    return 1;
}

static int Lua_CopyMatrix4( lua_State* l )
{
    const Mat4* source = CheckMatrix4FromLua(l, 1);
    CopyUserDataToLua(l, MATRIX_TYPE, sizeof(Mat4), source);
    return 1;
}

static int Lua_MultiplyMatrix4( lua_State* l )
{
    const Mat4* a = CheckMatrix4FromLua(l, 1);
    const Mat4* b = CheckMatrix4FromLua(l, 2);
    Mat4* r = (Mat4*)PushUserDataToLua(l, MATRIX_TYPE, sizeof(Mat4));
    *r = MulMat4(*a, *b);
    return 1;
}

static int Lua_TranslateMatrix4( lua_State* l )
{
    const Mat4* a = CheckMatrix4FromLua(l, 1);
    const Vec3 t = CreateVec3(luaL_checknumber(l, 2),
                              luaL_checknumber(l, 3),
                              luaL_checknumber(l, 4));

    Mat4* destination = (Mat4*)PushUserDataToLua(l, MATRIX_TYPE, sizeof(Mat4));
    *destination = TranslateMat4(*a, t);
    return 1;
}

static int Lua_ScaleMatrix4( lua_State* l )
{
    const Mat4* a = CheckMatrix4FromLua(l, 1);
    const Vec3 s = CreateVec3(luaL_checknumber(l, 2),
                              luaL_checknumber(l, 3),
                              luaL_checknumber(l, 4));

    Mat4* destination = (Mat4*)PushUserDataToLua(l, MATRIX_TYPE, sizeof(Mat4));
    *destination = ScaleMat4(*a, s);
    return 1;
}

static int Lua_RotateMatrix4( lua_State* l )
{
    const Mat4* a = CheckMatrix4FromLua(l, 1);

    const float angle = luaL_checknumber(l, 2);

    const Vec3 axis = CreateVec3(luaL_checknumber(l, 3),
                                 luaL_checknumber(l, 4),
                                 luaL_checknumber(l, 5));

    Mat4* destination = (Mat4*)PushUserDataToLua(l, MATRIX_TYPE, sizeof(Mat4));
    *destination = RotateMat4ByAngleAndAxis(*a, angle, axis);
    return 1;
}

static int Lua_Matrix4TransformVector( lua_State* l )
{
    const Mat4* a = CheckMatrix4FromLua(l, 1);
    const Vec3 v = CreateVec3(luaL_checknumber(l, 2),
                              luaL_checknumber(l, 3),
                              luaL_checknumber(l, 4));

    const Vec3 r = MulMat4ByVec3(*a, v);
    lua_pushnumber(l, r._[0]);
    lua_pushnumber(l, r._[1]);
    lua_pushnumber(l, r._[2]);
    return 3;
}

static int Lua_CreateLookAtMatrix( lua_State* l )
{
    const Vec3 eye = CreateVec3(luaL_checknumber(l, 1),
                                luaL_checknumber(l, 2),
                                luaL_checknumber(l, 3));

    const Vec3 center = CreateVec3(luaL_checknumber(l, 4),
                                   luaL_checknumber(l, 5),
                                   luaL_checknumber(l, 6));

    const Vec3 up = CreateVec3(luaL_checknumber(l, 7),
                               luaL_checknumber(l, 8),
                               luaL_checknumber(l, 9));

    Mat4* destination = (Mat4*)PushUserDataToLua(l, MATRIX_TYPE, sizeof(Mat4));
    *destination = CreateLookAtMat4(eye, center, up);
    return 1;
}

static int Lua_ClipTranslationOfMatrix4( lua_State* l )
{
    const Mat4* a = CheckMatrix4FromLua(l, 1);

    Mat4* destination = (Mat4*)PushUserDataToLua(l, MATRIX_TYPE, sizeof(Mat4));
    *destination = ClipTranslationOfMat4(*a);
    return 1;
}

Mat4* CreateMatrix4InLua( lua_State* l )
{
    return (Mat4*)PushUserDataToLua(l, MATRIX_TYPE, sizeof(Mat4));
}

Mat4* GetMatrix4FromLua( lua_State* l, int stackPosition )
{
    return (Mat4*)GetUserDataFromLua(l, stackPosition, MATRIX_TYPE);
}

Mat4* CheckMatrix4FromLua( lua_State* l, int stackPosition )
{
    return (Mat4*)CheckUserDataFromLua(l, stackPosition, MATRIX_TYPE);
}

int CheckTransformationFlagsFromLua( lua_State* l, int stackPosition )
{
    const char* flagString = luaL_checkstring(l, stackPosition);
    int flags = 0;
    for(const char* c = flagString; *c != '\0'; c++)
    {
        switch(*c)
        {
            case 'r':
                flags |= COPY_ROTATION;
                break;

            case 't':
                flags |= COPY_TRANSLATION;
                break;

            default:
                FatalError("Unknown flag: %c in '%s'", *c, flagString);
        }
    }
    return flags;
}


// --- Register in Lua ---

void RegisterMathInLua()
{
    RegisterUserDataTypeInLua(QUATERNION_TYPE, NULL);
    RegisterFunctionInLua("CreateQuaternion", Lua_CreateQuaternion);
    RegisterFunctionInLua("CreateQuaternionByAngleAndAxis", Lua_CreateQuaternionByAngleAndAxis);
    RegisterFunctionInLua("CopyQuaternion", Lua_CopyQuaternion);
    RegisterFunctionInLua("NormalizeQuaternion", Lua_NormalizeQuaternion);
    RegisterFunctionInLua("QuaternionConjugate", Lua_QuaternionConjugate);
    RegisterFunctionInLua("InvertQuaternion", Lua_InvertQuaternion);
    RegisterFunctionInLua("MultiplyQuaternion", Lua_MultiplyQuaternion);
    RegisterFunctionInLua("QuaternionXVector3", Lua_QuaternionXVector3);

    RegisterUserDataTypeInLua(MATRIX_TYPE, NULL);
    RegisterFunctionInLua("CreateMatrix4", Lua_CreateMatrix4);
    RegisterFunctionInLua("CreateMatrix4FromQuaternion", Lua_CreateMatrix4FromQuaternion);
    RegisterFunctionInLua("CopyMatrix4", Lua_CopyMatrix4);
    RegisterFunctionInLua("MultiplyMatrix4", Lua_MultiplyMatrix4);
    RegisterFunctionInLua("TranslateMatrix4", Lua_TranslateMatrix4);
    RegisterFunctionInLua("ScaleMatrix4", Lua_ScaleMatrix4);
    RegisterFunctionInLua("RotateMatrix4", Lua_RotateMatrix4);
    RegisterFunctionInLua("Matrix4TransformVector", Lua_Matrix4TransformVector);
    RegisterFunctionInLua("CreateLookAtMatrix", Lua_CreateLookAtMatrix);
    RegisterFunctionInLua("ClipTranslationOfMatrix4", Lua_ClipTranslationOfMatrix4);
}
