#ifndef __MESH__
#define __MESH__

#include "Vertex.h"

struct Mesh
{
    Vertex* vertices;
    int vertexCount;

    unsigned short* indices;
    int indexCount;

    static bool Load( Mesh* mesh, const char* file );
    static void Free( const Mesh* mesh );
};

#endif
