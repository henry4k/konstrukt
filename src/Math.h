#ifndef __MATH__
#define __MATH__

#include <math.h>
#include <float.h>

//#define GLM_FORCE_CXX98 // Portability
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

bool collides( glm::vec3* out, glm::vec3 aPosition, glm::vec3 aHalfWidth, glm::vec3 bPosition, glm::vec3 bHalfWidth );

#endif
