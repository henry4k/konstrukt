#ifndef __MESH__
#define __MESH__

#include "OpenGL.h"
#include "MeshBuffer.h"

struct Mesh
{
    GLuint vertexBuffer;
    GLuint indexBuffer;
    int primitiveType;
    int size;
};

bool CreateMesh( Mesh* mesh, const MeshBuffer* buffer );
void FreeMesh( const Mesh* mesh );
void DrawMesh( const Mesh* mesh );

#endif
