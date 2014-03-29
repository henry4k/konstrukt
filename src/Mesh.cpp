#include "Common.h"
#include "OpenGL.h"
#include "Squirrel.h"
#include "Mesh.h"

bool CreateMesh( Mesh* mesh, const MeshBuffer* buffer )
{
    assert(mesh);
    assert(buffer);
    if(buffer->vertices.empty() == 0)
        Error("Creating an empty mesh.");

    mesh->primitiveType = GL_TRIANGLES; // Default to triangles (can be changed later)

    glGenBuffers(1, &mesh->vertexBuffer);

    glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, buffer->vertices.size()*sizeof(Vertex), &buffer->vertices[0], GL_STATIC_DRAW);

    if(buffer->indices.size() > 0)
    {
        glGenBuffers(1, &mesh->indexBuffer);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, buffer->indices.size()*sizeof(unsigned short), &buffer->indices[0], GL_STATIC_DRAW);

        mesh->size = buffer->indices.size();
    }
    else
    {
        mesh->size = buffer->vertices.size();
    }

    return true;
}

void FreeMesh( const Mesh* mesh )
{
    glDeleteBuffers(1, &mesh->vertexBuffer);

    if(mesh->indexBuffer)
        glDeleteBuffers(1, &mesh->indexBuffer);
}

void DrawMesh( const Mesh* mesh )
{
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
    SetVertexAttributePointers(NULL);

    if(mesh->indexBuffer)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
        glDrawElements(mesh->primitiveType, mesh->size, GL_UNSIGNED_SHORT, 0);
    }
    else
    {
        glDrawArrays(mesh->primitiveType, 0, mesh->size);
    }
}



// --- Squirrel Bindings ---

SQInteger OnReleaseMesh( void* userData, SQInteger size )
{
    FreeMesh((Mesh*)userData); // Some compilers can't cast pointers directly to smaller data types.
    return 1;
}

SQInteger Squirrel_CreateMesh( HSQUIRRELVM vm )
{
    Mesh* mesh = NULL;
    sq_getuserdata(vm, 2, (void**)&mesh, NULL);

    Mesh* mesh = (Mesh*)CreateUserDataInSquirrel(vm, sizeof(Mesh), OnReleaseMesh);
    if(!CreateMesh(mesh, mesh))
    {
        sq_pop(vm, 1);
        return 0;
    }
    else
    {
        return 1;
    }
}
RegisterStaticFunctionInSquirrel(CreateMesh, 2, ".u");
