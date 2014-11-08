#ifndef __APOAPSIS_MATH__
#define __APOAPSIS_MATH__

#include <math.h>
#include <float.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_CXX11
#define GLM_FORCE_ONLY_XYZW
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>


#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

static const double PI = M_PI;
static const double TAU = PI*2.0;

enum TransformationCopyFlags
{
    COPY_ROTATION = (1 << 0),
    COPY_TRANSLATION = (1 << 1)
};


glm::mat4 MakeRotationMatrix( glm::mat4 m );

glm::mat4 GetTransformation( glm::mat4 transformation, int flags );

#endif
