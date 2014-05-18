#include "Common.h"
#include "Lua.h"
#include "Math.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/projection.hpp>

using namespace glm;

//#define DEBUG_LOG(...) Log(__VA_ARGS__)
#define DEBUG_LOG(...)

#define DEBUG_LOG_VEC3(V) DEBUG_LOG("%20s = { x=%.2f, y=%.2f, z=%.2f }", #V, V.x, V.y, V.z)
#define DEBUG_LOG_IVEC3(V) DEBUG_LOG("%20s = { x=%d, y=%d, z=%d }", #V, V.x, V.y, V.z)

mat4 MakeRotationMatrix( mat4 m )
{
    mat4 r = m;
    r[0].w = 0;
    r[1].w = 0;
    r[2].w = 0;
    r[3] = vec4(0,0,0,1);
    return r;
}

bool TestAabbOverlap( Box a, Box b )
{
    for(int i = 0; i < 3; ++i)
    {
        const float overlap = (a.halfWidth[i]+b.halfWidth[i]) - abs(a.position[i]-b.position[i]);
        if(overlap < 0)
            return false;
    }
    return true;
}

bool TestAabbOverlap( Box a, Box b, vec3* penetrationOut )
{
    vec3 side;
    vec3 overlap;
    for(int i = 0; i < 3; ++i)
    {
        overlap[i] = (a.halfWidth[i]+b.halfWidth[i]) - abs(a.position[i]-b.position[i]);
        if(overlap[i] < 0)
            return false;

        side[i] = sign(a.position[i]-b.position[i]);
    }

    if(overlap.x < overlap.y && overlap.x < overlap.z)
    {
        *penetrationOut = vec3(overlap.x*side.x, 0, 0);
    }
    else if(overlap.y < overlap.x && overlap.y < overlap.z)
    {
        *penetrationOut = vec3(0, overlap.y*side.y, 0);
    }
    else
    {
        *penetrationOut = vec3(0, 0, overlap.z*side.z);
    }

    if(penetrationOut->length() >= 0.01f)
    {
        DEBUG_LOG("penetration = %.2f|%.2f|%.2f",
            penetrationOut->x,
            penetrationOut->y,
            penetrationOut->z
        );
    }

    return true;
}

/* TODO
Box GetSweptBroadphaseBox( Box b )
{
    Box broadphasebox;
    memset(&broadphasebox, 0, sizeof(broadphasebox));

    for(int i = 0; i < 3; ++i)
    {
        if(b.velocity[i] > 0)
        {
            broadphasebox.position[i] = b.position[i];
            broadphasebox.size[i]     = b.size[i] + b.velocity[i];
        }
        else
        {
            broadphasebox.position[i] = b.position[i] + b.velocity[i];
            broadphasebox.size[i]     = b.size[i] - b.velocity[i];
        }
    }

    broadphasebox.x = b.vx > 0 ? b.x : b.x + b.vx;
    broadphasebox.w = b.vx > 0 ? b.vx + b.w : b.w - b.vx;

    return broadphasebox;
}
*/

float SweptAabb( Box a, Box b, vec3* normalOut, float timeFrame )
{
    DEBUG_LOG("=============================");

    DEBUG_LOG("a.position.x = %.2f, min = %.2f, max = %.2f", a.position.x, a.position.x-a.halfWidth.x, a.position.x+a.halfWidth.x);
    DEBUG_LOG("b.position.x = %.2f, min = %.2f, max = %.2f", b.position.x, b.position.x-b.halfWidth.x, b.position.x+b.halfWidth.x);

    /*
    // find the distance between the objects on the near and far sides for both x and y
         F   NN F
         [   ][ ]
    01234567890123456
             <>    <- InvEntry
         <------>  <- InvExit
    */

    vec3 farDelta  = abs(b.position-a.position) + (b.halfWidth+a.halfWidth);
    vec3 nearDelta = abs(b.position-a.position) - (b.halfWidth+a.halfWidth);

    DEBUG_LOG("farDelta.x = %.2f", farDelta.x);
    DEBUG_LOG("nearDelta.x = %.2f", nearDelta.x);


    // find time of collision and time of leaving for each axis (if statement is to prevent divide by zero)

    const vec3 relativeVelocity = a.velocity - b.velocity;
    DEBUG_LOG("relativeddVelocity.x = %.2f", relativeVelocity.x);

    vec3 axisEntryTime;
    vec3 axisExitTime;

    for(int i = 0; i < 3; ++i)
    {
        if(relativeVelocity[i] == 0)
        {
            axisEntryTime[i] = -std::numeric_limits<float>::infinity();
            axisExitTime[i]  = +std::numeric_limits<float>::infinity();
        }
        else
        {
            axisEntryTime[i] = nearDelta[i] / relativeVelocity[i];
            axisExitTime[i]  =  farDelta[i] / relativeVelocity[i];
        }

        DEBUG_LOG("axisEntryTime[%d] = %.2f", i, axisEntryTime[i]);
        DEBUG_LOG(" axisExitTime[%d] = %.2f", i, axisExitTime[i]);
    }


    // find the earliest/latest times of collision (on each axis)
    float entryTime = max(axisEntryTime.x, max(axisEntryTime.y, axisEntryTime.z));
    float exitTime  = min(axisExitTime.x, min(axisExitTime.y, axisExitTime.z));

    DEBUG_LOG("entryTime = %.2f", entryTime);
    DEBUG_LOG(" exitTime = %.2f", exitTime);


    // if there was no collision
    if( entryTime > exitTime ||
        (
            axisEntryTime.x < 0 &&
            axisEntryTime.y < 0 &&
            axisEntryTime.z < 0
        ) ||
        (
            axisEntryTime.x > timeFrame ||
            axisEntryTime.y > timeFrame ||
            axisEntryTime.z > timeFrame
        )
    )
    {
        DEBUG_LOG("No collision!");
        *normalOut = vec3(0,0,0);
        return timeFrame;
    }
    else
    {
        // calculate normal of collided surface

        // x axis is greatest
        if(axisEntryTime.x > axisEntryTime.y && axisEntryTime.x > axisEntryTime.z)
        {
            *normalOut = vec3(-sign(farDelta.x),0,0);
        }
        // y axis is greatest
        else if(axisEntryTime.y > axisEntryTime.x && axisEntryTime.y > axisEntryTime.z)
        {
            *normalOut = vec3(0,-sign(farDelta.y),0);
        }
        // z axis is greatest
        else
        {
            *normalOut = vec3(0,0,-sign(farDelta.z));
        }

        DEBUG_LOG("=========== Collision! =============");
        return entryTime;
    }
}


bool RayTestAabb( const Ray& ray, Aabb aabb, float* lengthOut )
{
    DEBUG_LOG_VEC3(ray.origin);
    DEBUG_LOG_VEC3(ray.direction);
    DEBUG_LOG_VEC3(ray.inverseDirection);
    DEBUG_LOG_IVEC3(ray.sign);

    DEBUG_LOG_VEC3(aabb.position);
    DEBUG_LOG_VEC3(aabb.halfWidth);

    const vec3 bounds[2] = {
        aabb.position - aabb.halfWidth, // min
        aabb.position + aabb.halfWidth  // max
    };
    DEBUG_LOG_VEC3(bounds[0]);
    DEBUG_LOG_VEC3(bounds[1]);

    float tmin = (bounds[  ray.sign.x].x - ray.origin.x) * ray.inverseDirection.x;
    float tmax = (bounds[1-ray.sign.x].x - ray.origin.x) * ray.inverseDirection.x;
    DEBUG_LOG("tmin = %.2f", tmin);
    DEBUG_LOG("tmax = %.2f", tmax);

    const float tymin = (bounds[  ray.sign.y].y - ray.origin.y) * ray.inverseDirection.y;
    const float tymax = (bounds[1-ray.sign.y].y - ray.origin.y) * ray.inverseDirection.y;
    DEBUG_LOG("tymin = %.2f", tymin);
    DEBUG_LOG("tymax = %.2f", tymax);
    if(tmin > tymax || tymin > tmax)
        return false;
    if(tymin > tmin)
        tmin = tymin;
    if(tymax < tmax)
        tmax = tymax;

    const float tzmin = (bounds[  ray.sign.z].z - ray.origin.z) * ray.inverseDirection.z;
    const float tzmax = (bounds[1-ray.sign.z].z - ray.origin.z) * ray.inverseDirection.z;
    DEBUG_LOG("tzmin = %.2f", tzmin);
    DEBUG_LOG("tzmax = %.2f", tzmax);
    if(tmin > tzmax || tzmin > tmax)
        return false;
    if(tzmin > tmin)
        tmin = tzmin;
    if(tzmax < tmax)
        tmax = tzmax;

    DEBUG_LOG("result min = %f", tmin);
    DEBUG_LOG("result max = %f", tmax);
    *lengthOut = tmin;
    return true;
}


// --- lua bindings ---

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

AutoRegisterInLua()
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
