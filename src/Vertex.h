#ifndef __VERTEX__
#define __VERTEX__

#include "Common.h"
#include "Math.h"

struct Vertex
{
    glm::vec3 position;
    glm::vec2 texCoord;
    glm::vec3 normal;
    glm::vec4 tangent;

    static void EnableVertexArrays();
    static void BindAttributes( Handle shader );
    static void SetAttributePointers();
};

#endif
