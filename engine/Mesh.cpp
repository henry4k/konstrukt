#include <string.h> // memset

#include "Common.h"
#include "OpenGL.h"
#include "MeshBuffer.h"
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
    const int vertexCount = GetMeshBufferVertexCount(buffer);
    const int indexCount = GetMeshBufferIndexCount(buffer);

    if(!vertexCount)
        Error("Creating an empty mesh.");

    Mesh* mesh = new Mesh;
    memset(mesh, 0, sizeof(Mesh));

    InitReferenceCounter(&mesh->refCounter);

    mesh->primitiveType = GL_TRIANGLES; // Default to triangles (can be changed later)

    glGenBuffers(1, &mesh->vertexBuffer);

    glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER,
                 vertexCount*sizeof(Vertex),
                 GetMeshBufferVertices(buffer),
                 GL_STATIC_DRAW);

    if(indexCount)
    {
        glGenBuffers(1, &mesh->indexBuffer);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     indexCount*sizeof(unsigned short),
                     GetMeshBufferIndices(buffer),
                     GL_STATIC_DRAW);

        mesh->size = indexCount;
    }
    else
    {
        mesh->size = vertexCount;
    }

    return mesh;
}

static void BindMesh( const Mesh* mesh )
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

static const Mesh* CurrentMesh = NULL;

void DrawMesh( const Mesh* mesh )
{
    if(mesh != CurrentMesh)
    {
        BindMesh(mesh);
        CurrentMesh = mesh;
    }

    if(mesh->indexBuffer)
        glDrawElements(mesh->primitiveType, mesh->size, GL_UNSIGNED_SHORT, 0);
    else
        glDrawArrays(mesh->primitiveType, 0, mesh->size);
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
