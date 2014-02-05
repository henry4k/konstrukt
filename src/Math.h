#ifndef __MATH__
#define __MATH__

#include <math.h>
#include <float.h>

//#define GLM_FORCE_CXX98 // Portability
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>


struct Aabb
{
    glm::vec3 position;
    glm::vec3 halfWidth;
};

struct Box
{
    glm::vec3 position;
    glm::vec3 halfWidth;
    glm::vec3 velocity;
};

bool TestAabbOverlap( Box a, Box b );
bool TestAabbOverlap( Box a, Box b, glm::vec3* penetrationOut );
float SweptAabb( Box a, Box b, glm::vec3* normalOut, float timeStep );

struct Ray
{
    glm::vec3 origin;
    glm::vec3 direction;
    glm::vec3 inverseDirection;
    glm::ivec3 sign;

    Ray( glm::vec3 o, glm::vec3 d ) :
        origin(o),
        direction(glm::normalize(d)),
        inverseDirection(1.0f/direction),
        sign(
            (inverseDirection.x < 0) ? 1 : 0,
            (inverseDirection.y < 0) ? 1 : 0,
            (inverseDirection.z < 0) ? 1 : 0
        )
    {
    }
};

bool RayTestAabb( const Ray& ray, Aabb aabb, float* lengthOut );

#endif
