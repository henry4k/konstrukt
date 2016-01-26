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
#if defined(APOAPSIS_DEBUG_MESH)
    GLuint debugVertexBuffer;
    int debugVertexCount;
#endif
};


#if defined(APOAPSIS_DEBUG_MESH)
static void BuildDebugMesh( const MeshBuffer* buffer, Mesh* mesh );
static void DrawDebugMesh( const Mesh* mesh );
#endif


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

#if defined(APOAPSIS_DEBUG_MESH)
    BuildDebugMesh(buffer, mesh);
#endif

    return mesh;
}

static void BindMesh( const Mesh* mesh )
{
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);

    if(mesh->indexBuffer)
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);

    SetVertexAttributePointers(NULL);
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

#if defined(APOAPSIS_DEBUG_MESH)
    DrawDebugMesh(mesh);
#endif
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


#if defined(APOAPSIS_DEBUG_MESH)
const int DebugVerticesPerVertex = 6;
const Vec3 Red   = {{1,0,0}};
const Vec3 Green = {{0,1,0}};
const Vec3 Blue  = {{0,0,1}};

static void CalcDebugVertices( const Vertex* input, Vertex* outputs )
{
    const float scale = 0.03f;

    outputs[0].position = input->position;
    REPEAT(3,i) { outputs[1].position._[i] = input->position._[i] + input->normal._[i]    * scale; }
    outputs[1].color = Blue;

    outputs[2].position = input->position;
    REPEAT(3,i) { outputs[3].position._[i] = input->position._[i] + input->tangent._[i]   * scale; }
    outputs[3].color = Red;

    outputs[4].position = input->position;
    REPEAT(3,i) { outputs[5].position._[i] = input->position._[i] + input->bitangent._[i] * scale; }
    outputs[5].color = Green;
}

static void BuildDebugMesh( const MeshBuffer* buffer, Mesh* mesh )
{
    const int vertexCount      = GetMeshBufferVertexCount(buffer);
    const int indexCount       = GetMeshBufferIndexCount(buffer);
    const Vertex* vertices     = GetMeshBufferVertices(buffer);
    const VertexIndex* indices = GetMeshBufferIndices(buffer);

    const int elementCount = (indexCount > 0) ? indexCount : vertexCount;
    const int debugVertexCount = elementCount * DebugVerticesPerVertex;
    Vertex* debugVertices = new Vertex[debugVertexCount];
    memset(debugVertices, 0, sizeof(Vertex)*debugVertexCount);

    if(indexCount > 0)
    {
        for(int i = 0; i < indexCount; i++)
        {
            const VertexIndex index = indices[i];
            const Vertex* input = &vertices[index];
            Vertex* outputs = &debugVertices[i*DebugVerticesPerVertex];
            CalcDebugVertices(input, outputs);
        }
    }
    else
    {
        for(int i = 0; i < vertexCount; i++)
        {
            const Vertex* input = &vertices[i];
            Vertex* outputs = &debugVertices[i*DebugVerticesPerVertex];
            CalcDebugVertices(input, outputs);
        }
    }

    glGenBuffers(1, &mesh->debugVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->debugVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER,
                 debugVertexCount*sizeof(Vertex),
                 debugVertices,
                 GL_STATIC_DRAW);

    delete[] debugVertices;

    mesh->debugVertexCount = debugVertexCount;
}

static void DrawDebugMesh( const Mesh* mesh )
{
    CurrentMesh = NULL; // Make sure the render function always gets called.
    glBindBuffer(GL_ARRAY_BUFFER, mesh->debugVertexBuffer);
    SetVertexAttributePointers(NULL);
    glDrawArrays(GL_LINES, 0, mesh->debugVertexCount);
}
#endif
