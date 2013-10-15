#ifndef __MODEL__
#define __MODEL__

#include "OpenGL.h"
#include "Mesh.h"

struct Model
{
    GLuint vertexBuffer;
    GLuint indexBuffer;
    int primitiveType;
    int size;
};

bool CreateModel( Model* model, const Mesh* mesh );
bool LoadModel( Model* model, const char* mesh );
void FreeModel( const Model* model );
void DrawModel( const Model* model );

#endif
