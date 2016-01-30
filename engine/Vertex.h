#ifndef __KONSTRUKT_VERTEX__
#define __KONSTRUKT_VERTEX__

#include "Math.h"


typedef unsigned short VertexIndex;


struct Vertex
{
    Vec3 position;
    Vec3 color;
    Vec2 texCoord;
    Vec3 normal;
    Vec3 tangent;
    Vec3 bitangent;
};


void EnableVertexArrays();
void BindVertexAttributes( unsigned int programHandle );
void SetVertexAttributePointers( const void* data );
void CalcTriangleTangents( Vertex* a, Vertex* b, Vertex* c );
void CalcTriangleNormal( Vertex* a, Vertex* b, Vertex* c );

#endif
