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


glm::mat4 MakeRotationMatrix( glm::mat4 m );

#endif
