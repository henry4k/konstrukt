#ifndef __MESH__
#define __MESH__

#include "Vertex.h"

struct Mesh
{
    Vertex* vertices;
    int vertexCount;

    unsigned short* indices;
    int indexCount;
};

bool LoadMesh( Mesh* mesh, const char* file );
void FreeMesh( const Mesh* mesh );

#endif
