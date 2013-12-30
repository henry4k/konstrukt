#include "Common.h"
#include "Squirrel.h"
#include "Math.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/projection.hpp>

using namespace glm;


bool TestAABBOverlap( Box a, Box b )
{
    for(int i = 0; i < 3; ++i)
    {
        const float overlap = (a.halfWidth[i]+b.halfWidth[i]) - abs(a.position[i]-b.position[i]);
        if(overlap < 0)
            return false;
    }
    return true;
}

bool TestAABBOverlap( Box a, Box b, vec3* penetrationOut )
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
    Log("penetration = %.2f|%.2f|%.2f",
        penetrationOut->x,
        penetrationOut->y,
        penetrationOut->z
    );

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

float SweptAABB( Box a, Box b, vec3* normalOut, float timeFrame )
{
    Log("=============================");

    Log("a.position.x = %.2f, min = %.2f, max = %.2f", a.position.x, a.position.x-a.halfWidth.x, a.position.x+a.halfWidth.x);
    Log("b.position.x = %.2f, min = %.2f, max = %.2f", b.position.x, b.position.x-b.halfWidth.x, b.position.x+b.halfWidth.x);

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

    Log("farDelta.x = %.2f", farDelta.x);
    Log("nearDelta.x = %.2f", nearDelta.x);


    // find time of collision and time of leaving for each axis (if statement is to prevent divide by zero)

    const vec3 relativeVelocity = a.velocity - b.velocity;
    Log("relativeddVelocity.x = %.2f", relativeVelocity.x);

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

        Log("axisEntryTime[%d] = %.2f", i, axisEntryTime[i]);
        Log(" axisExitTime[%d] = %.2f", i, axisExitTime[i]);
    }


    // find the earliest/latest times of collision (on each axis)
    float entryTime = max(axisEntryTime.x, max(axisEntryTime.y, axisEntryTime.z));
    float exitTime  = min(axisExitTime.x, min(axisExitTime.y, axisExitTime.z));

    Log("entryTime = %.2f", entryTime);
    Log(" exitTime = %.2f", exitTime);


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
        Log("No collision!");
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

        Log("=========== Collision! =============");
        return entryTime;
    }
}


// --- Squirrel Bindings ---

SQInteger Squirrel_CreateMatrix4( HSQUIRRELVM vm )
{
    const mat4 matrix(1.0f);
    PushUserDataToSquirrel(vm, &matrix, sizeof(matrix), NULL);
    return 1;
}
RegisterStaticFunctionInSquirrel(CreateMatrix4, 1, ".");

SQInteger Squirrel_CopyMatrix4( HSQUIRRELVM vm )
{
    mat4* matrix;
    sq_getuserdata(vm, 2, (SQUserPointer*)&matrix, NULL);
    PushUserDataToSquirrel(vm, matrix, sizeof(mat4), NULL);
    return 1;
}
RegisterStaticFunctionInSquirrel(CopyMatrix4, 2, ".u");

SQInteger Squirrel_AddMatrix4( HSQUIRRELVM vm )
{
    mat4* a;
    sq_getuserdata(vm, 2, (SQUserPointer*)&a, NULL);

    mat4* b;
    sq_getuserdata(vm, 3, (SQUserPointer*)&b, NULL);

    const mat4 r = *a + *b;
    PushUserDataToSquirrel(vm, &r, sizeof(r), NULL);
    return 1;
}
RegisterStaticFunctionInSquirrel(AddMatrix4, 3, ".uu");

SQInteger Squirrel_SubMatrix4( HSQUIRRELVM vm )
{
    mat4* a;
    sq_getuserdata(vm, 2, (SQUserPointer*)&a, NULL);

    mat4* b;
    sq_getuserdata(vm, 3, (SQUserPointer*)&b, NULL);

    const mat4 r = *a - *b;
    PushUserDataToSquirrel(vm, &r, sizeof(r), NULL);
    return 1;
}
RegisterStaticFunctionInSquirrel(SubMatrix4, 3, ".uu");

SQInteger Squirrel_MulMatrix4( HSQUIRRELVM vm )
{
    mat4* a;
    sq_getuserdata(vm, 2, (SQUserPointer*)&a, NULL);

    mat4* b;
    sq_getuserdata(vm, 3, (SQUserPointer*)&b, NULL);

    const mat4 r = *a * *b;
    PushUserDataToSquirrel(vm, &r, sizeof(r), NULL);
    return 1;
}
RegisterStaticFunctionInSquirrel(MulMatrix4, 3, ".uu");

SQInteger Squirrel_DivMatrix4( HSQUIRRELVM vm )
{
    mat4* a;
    sq_getuserdata(vm, 2, (SQUserPointer*)&a, NULL);

    mat4* b;
    sq_getuserdata(vm, 3, (SQUserPointer*)&b, NULL);

    const mat4 r = *a / *b;
    PushUserDataToSquirrel(vm, &r, sizeof(r), NULL);
    return 1;
}
RegisterStaticFunctionInSquirrel(DivMatrix4, 3, ".uu");

SQInteger Squirrel_TranslateMatrix4( HSQUIRRELVM vm )
{
    mat4* a;
    sq_getuserdata(vm, 2, (SQUserPointer*)&a, NULL);

    float x, y, z;
    sq_getfloat(vm, 3, &x);
    sq_getfloat(vm, 4, &y);
    sq_getfloat(vm, 5, &z);

    const vec3 t(x,y,z);

    mat4 r = translate(*a, t);
    PushUserDataToSquirrel(vm, &r, sizeof(r), NULL);
    return 1;
}
RegisterStaticFunctionInSquirrel(TranslateMatrix4, 5, ".ufff");

SQInteger Squirrel_ScaleMatrix4( HSQUIRRELVM vm )
{
    mat4* a;
    sq_getuserdata(vm, 2, (SQUserPointer*)&a, NULL);

    float x, y, z;
    sq_getfloat(vm, 3, &x);
    sq_getfloat(vm, 4, &y);
    sq_getfloat(vm, 5, &z);

    const vec3 s(x,y,z);

    mat4 r = scale(*a, s);
    PushUserDataToSquirrel(vm, &r, sizeof(r), NULL);
    return 1;
}
RegisterStaticFunctionInSquirrel(ScaleMatrix4, 5, ".ufff");

SQInteger Squirrel_RotateMatrix4( HSQUIRRELVM vm )
{
    mat4* a;
    sq_getuserdata(vm, 2, (SQUserPointer*)&a, NULL);

    float angle, x, y, z;
    sq_getfloat(vm, 3, &angle);
    sq_getfloat(vm, 4, &x);
    sq_getfloat(vm, 5, &y);
    sq_getfloat(vm, 6, &z);

    mat4 r = rotate(*a, angle, vec3(x,y,z));

    PushUserDataToSquirrel(vm, &r, sizeof(r), NULL);
    return 1;
}
RegisterStaticFunctionInSquirrel(RotateMatrix4, 6, ".uffff");
