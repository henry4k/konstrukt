#ifndef __MATH__
#define __MATH__

#include <math.h>
#include <float.h>

//#define GLM_FORCE_CXX98 // Portability
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Box
{
    glm::vec3 position;
    glm::vec3 halfWidth;
    glm::vec3 velocity;
};

bool TestAABBOverlap( Box a, Box b );
bool TestAABBOverlap( Box a, Box b, glm::vec3* penetrationOut );
float SweptAABB( Box a, Box b, glm::vec3* normalOut, float timeStep );

#endif
