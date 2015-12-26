#ifndef __APOAPSIS_MATH__
#define __APOAPSIS_MATH__

#include <math.h>
#include <float.h>

#include "Warnings.h"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_ONLY_XYZW
#define GLM_LEFT_HANDED
BEGIN_EXTERNAL_CODE
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtx/projection.hpp>
END_EXTERNAL_CODE


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
