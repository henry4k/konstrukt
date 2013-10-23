#include "Math.h"

using namespace glm;

inline bool collides( float* out, float aPosition, float aHalfWidth, float bPosition, float bHalfWidth )
{
    const float delta = aPosition - bPosition;
    const float result = (aHalfWidth+bHalfWidth) - glm::abs(delta);
    *out = (delta > 0) ? result : -result;
    return result > 0;
}

bool collides( vec3* out, vec3 aPosition, vec3 aHalfWidth, vec3 bPosition, vec3 bHalfWidth )
{
    vec3 result(0,0,0);

    if(collides(&result.x, aPosition.x, aHalfWidth.x, bPosition.x, bHalfWidth.x) &&
       collides(&result.y, aPosition.y, aHalfWidth.y, bPosition.y, bHalfWidth.y) &&
       collides(&result.z, aPosition.z, aHalfWidth.z, bPosition.z, bHalfWidth.z))
    {
        const vec3 absResult = glm::abs(result);

        if(absResult.x <= absResult.y && absResult.x <= absResult.z)
            *out = vec3(result.x, 0, 0);
        else if(absResult.y <= absResult.x && absResult.y <= absResult.z)
            *out = vec3(0, result.y, 0);
        else
            *out = vec3(0, 0, result.z);
        return true;
    }
    else
    {
        return false;
    }
}
