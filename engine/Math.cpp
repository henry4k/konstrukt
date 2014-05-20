#include "Common.h"
#include "Math.h"

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