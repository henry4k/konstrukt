#include <string.h> // memset

#include "Common.h"
#include "OpenGL.h"
#include "Reference.h"
#include "Mesh.h"


struct Mesh
{
    ReferenceCounter refCounter;
    GLuint vertexBuffer;
    GLuint indexBuffer;
    int primitiveType;
    int size;
};


Mesh* CreateMesh( const MeshBuffer* buffer )
{
    assert(buffer);
    if(buffer->vertices.empty())
        Error("Creating an empty mesh.");

    Mesh* mesh = new Mesh;
    memset(mesh, 0, sizeof(Mesh));

    InitReferenceCounter(&mesh->refCounter);

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

    return mesh;
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

static void FreeMesh( Mesh* mesh )
{
    FreeReferenceCounter(&mesh->refCounter);

    glDeleteBuffers(1, &mesh->vertexBuffer);

    if(mesh->indexBuffer)
        glDeleteBuffers(1, &mesh->indexBuffer);

    delete mesh;
}

void ReferenceMesh( Mesh* mesh )
{
    Reference(&mesh->refCounter);
}

void ReleaseMesh( Mesh* mesh )
{
    Release(&mesh->refCounter);
    if(!HasReferences(&mesh->refCounter))
        FreeMesh(mesh);
}