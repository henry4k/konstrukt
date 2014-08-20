#include "Math.h"

using namespace glm;


mat4 MakeRotationMatrix( mat4 m )
{
    mat4 r = m;
    r[0].w = 0;
    r[1].w = 0;
    r[2].w = 0;
    r[3] = vec4(0,0,0,1);
    return r;
}