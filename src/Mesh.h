#ifndef __MESH__
#define __MESH__

#include "OpenGL.h"
#include "MeshBuffer.h"
#include "Lua.h"

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

Mesh* GetMeshFromLua( lua_State* l, int stackPosition );
Mesh* CheckMeshFromLua( lua_State* l, int stackPosition );

#endif
