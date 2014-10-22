#include "Common.h"
#include "Math.h"
#include <glm/gtc/matrix_transform.hpp>

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

glm::mat4 GetTransformation( mat4 transformation, int flags )
{
    switch(flags)
    {
        case 0:
            return mat4(1);
        case COPY_ROTATION:
            return MakeRotationMatrix(transformation);
        case COPY_TRANSLATION:
        {
            const vec4 t = transformation * vec4(0,0,0,1);
            const vec3 p = vec3(t[0], t[1], t[2]) * t[3];
            return translate(mat4(1), p);
        }
        case COPY_ROTATION | COPY_TRANSLATION:
            return transformation;
        default:
            FatalError("Unknown copy flags.");
            return mat4(1);
    }
}
