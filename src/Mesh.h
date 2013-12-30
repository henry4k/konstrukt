#ifndef __MESH__
#define __MESH__

#include "Vertex.h"
#include "Squirrel.h"

struct Mesh
{
    Vertex* vertices;
    int vertexCount;

    unsigned short* indices;
    int indexCount;
};

bool LoadMesh( Mesh* mesh, const char* file );
void FreeMesh( const Mesh* mesh );

SQInteger OnReleaseMesh( void* userData, SQInteger size );

#endif
