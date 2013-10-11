#ifndef __MODEL__
#define __MODEL__

#include "Common.h"
#include "Mesh.h"

struct Model
{
    Handle vertexBuffer;
    Handle indexBuffer;
    int primitiveType;
    int size;
};

bool CreateModel( Model* model, const Mesh* mesh );
bool LoadModel( Model* model, const char* mesh );
void FreeModel( const Model* model );
void DrawModel( const Model* model );

#endif
