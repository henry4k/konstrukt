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

    static bool Create( Model* model, const Mesh* mesh );
    static bool Load( Model* model, const char* mesh );
    static void Free( const Model* model );
    static void Draw( const Model* model );
};

#endif
