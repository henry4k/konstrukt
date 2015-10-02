#ifndef __APOAPSIS_VERTEX__
#define __APOAPSIS_VERTEX__

#include "Math.h"


typedef unsigned short VertexIndex;


struct Vertex
{
    glm::vec3 position;
    glm::vec3 color;
    glm::vec2 texCoord;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec3 bitangent;
};


void EnableVertexArrays();
void BindVertexAttributes( unsigned int programHandle );
void SetVertexAttributePointers( const void* data );
void CalcTriangleTangents( Vertex* a, Vertex* b, Vertex* c );
void CalcTriangleNormal( Vertex* a, Vertex* b, Vertex* c );

#endif
