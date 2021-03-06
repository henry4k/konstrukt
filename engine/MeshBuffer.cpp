#include <vector>
#include <assert.h>
#include <stddef.h> // size_t
#include <math.h> // fabsf

#include "Common.h"
#include "Math.h"
#include "Reference.h"
#include "MeshBuffer.h"


struct MeshBuffer
{
    ReferenceCounter refCounter;
    std::vector<Vertex> vertices;
    std::vector<VertexIndex> indices;
    // TODO: Replace std::vector with Array!
};


MeshBuffer* CreateMeshBuffer()
{
    MeshBuffer* buffer = new MeshBuffer;
    InitReferenceCounter(&buffer->refCounter);
    return buffer;
}

static void FreeMeshBuffer( MeshBuffer* buffer )
{
    FreeReferenceCounter(&buffer->refCounter);
    delete buffer;
}

void ReferenceMeshBuffer( MeshBuffer* buffer )
{
    Reference(&buffer->refCounter);
}

void ReleaseMeshBuffer( MeshBuffer* buffer )
{
    Release(&buffer->refCounter);
    if(!HasReferences(&buffer->refCounter))
        FreeMeshBuffer(buffer);
}

void AddVertexToMeshBuffer( MeshBuffer* buffer, const Vertex* vertex )
{
    buffer->vertices.push_back(*vertex);
}

void AddIndexToMeshBuffer( MeshBuffer* buffer, VertexIndex index )
{
    buffer->indices.push_back(index);
}

static void TransformMeshBufferRange( MeshBuffer* buffer, Mat4 transformation, int firstVertex, int vertexCount )
{
    assert(firstVertex >= 0);
    assert(vertexCount >= 0);
    assert(firstVertex+vertexCount <= (int)buffer->vertices.size());

    const Mat4 rotation = ClipTranslationOfMat4(transformation);

    Vertex* vertex = &buffer->vertices[firstVertex];
    const Vertex* end = &buffer->vertices[firstVertex+vertexCount];
    for(; vertex != end; ++vertex)
    {
        vertex->position  = MulMat4ByVec3(transformation, vertex->position);
        vertex->normal    = NormalizeVec3(MulMat4ByVec3(rotation, vertex->normal));
        vertex->tangent   = NormalizeVec3(MulMat4ByVec3(rotation, vertex->tangent));
        vertex->bitangent = NormalizeVec3(MulMat4ByVec3(rotation, vertex->bitangent));
    }
}

void TransformMeshBuffer( MeshBuffer* buffer, Mat4 transformation )
{
    TransformMeshBufferRange(buffer, transformation, 0, buffer->vertices.size());
}

void AppendMeshBuffer( MeshBuffer* buffer, const MeshBuffer* otherBuffer, const Mat4* transformation )
{
    // TODO: Raise error if target buffer doesn't use indices, but source buffer does.
    // TODO: Generate indices if target buffer uses them, but source buffer doesn't.

    buffer->indices.reserve(buffer->indices.size()+otherBuffer->indices.size());
    const VertexIndex indexOffset = buffer->vertices.size();
    for(size_t i = 0; i < otherBuffer->indices.size(); i++)
    {
        const VertexIndex index = otherBuffer->indices[i];
        buffer->indices.push_back(index+indexOffset);
    }

    const int start = buffer->vertices.size();
    buffer->vertices.insert(
        buffer->vertices.end(),
        otherBuffer->vertices.begin(),
        otherBuffer->vertices.end()
    );
    if(transformation)
        TransformMeshBufferRange(buffer, *transformation, start, otherBuffer->vertices.size());
}

int GetMeshBufferVertexCount( const MeshBuffer* buffer )
{
    return buffer->vertices.size();
}

const Vertex* GetMeshBufferVertices( const MeshBuffer* buffer )
{
    return &buffer->vertices[0];
}

int GetMeshBufferIndexCount( const MeshBuffer* buffer )
{
    return buffer->indices.size();
}

const VertexIndex* GetMeshBufferIndices( const MeshBuffer* buffer )
{
    return &buffer->indices[0];
}


// ---- mesh reindexing ----

const float Epsilon = 0.001;

static bool IsNearlyEqual( float a, float b )
{
    const float delta = fabsf(a-b);
    return delta <= Epsilon;
}

static bool IsNearlyEqualVec2( Vec2 a, Vec2 b )
{
    return IsNearlyEqual(a._[0], b._[0]) &&
           IsNearlyEqual(a._[1], b._[1]);
}

static bool IsNearlyEqualVec3( Vec3 a, Vec3 b )
{
    return IsNearlyEqual(a._[0], b._[0]) &&
           IsNearlyEqual(a._[1], b._[1]) &&
           IsNearlyEqual(a._[2], b._[2]);
}

static int FindSimilarVertex( const Vertex* vertices,
                              int vertexCount,
                              const Vertex* reference )
{
    for(int i = 0; i < vertexCount; i++)
    {
        const Vertex* vertex = &vertices[i];
        if(IsNearlyEqualVec3(vertex->position, reference->position) &&
           IsNearlyEqualVec3(vertex->color,    reference->color) &&
           IsNearlyEqualVec2(vertex->texCoord, reference->texCoord) &&
           IsNearlyEqualVec3(vertex->normal,   reference->normal))
        {
            return i;
        }
    }
    return -1;
}

static void IndexVertex( const Vertex* vertex,
                         Vertex* newVertices,
                         VertexIndex* newIndices,
                         int* newVertexCount,
                         int* newIndexCount )
{
    const int index = FindSimilarVertex(newVertices,
                                        *newVertexCount,
                                        vertex);
    if(index >= 0)
    {
        newIndices[*newIndexCount] = (VertexIndex)index;
        *newIndexCount = *newIndexCount + 1;

        // Average tangents:
        REPEAT(3,i) { newVertices[index].tangent._[i]   += vertex->tangent._[i]; }
        REPEAT(3,i) { newVertices[index].bitangent._[i] += vertex->bitangent._[i]; }
    }
    else
    {
        newIndices[*newIndexCount] = (VertexIndex)*newVertexCount;
        *newIndexCount = *newIndexCount + 1;

        newVertices[*newVertexCount] = *vertex;
        *newVertexCount = *newVertexCount + 1;
    }
}

static void IndexMeshBuffer( MeshBuffer* buffer )
{
    const Vertex* vertices     = GetMeshBufferVertices(buffer);
    const VertexIndex* indices = GetMeshBufferIndices(buffer);
    const int vertexCount      = GetMeshBufferVertexCount(buffer);
    const int indexCount       = GetMeshBufferIndexCount(buffer);

    Vertex* newVertices     = NEW_ARRAY(Vertex, vertexCount);
    VertexIndex* newIndices = NEW_ARRAY(VertexIndex, vertexCount);
    int newVertexCount = 0;
    int newIndexCount  = 0;

    if(indexCount > 0)
    {
        REPEAT(indexCount, i)
        {
            const int index = indices[i];
            const Vertex* vertex = &vertices[index];
            IndexVertex(vertex,
                        newVertices,
                        newIndices,
                        &newVertexCount,
                        &newIndexCount);
        }
    }
    else
    {
        REPEAT(indexCount, i)
        {
            const Vertex* vertex = &vertices[i];
            IndexVertex(vertex,
                        newVertices,
                        newIndices,
                        &newVertexCount,
                        &newIndexCount);
        }
    }

    buffer->vertices.assign(newVertices, newVertices+newVertexCount);
    buffer->indices.assign(newIndices, newIndices+newIndexCount);

    DELETE_ARRAY(newVertices, vertexCount);
    DELETE_ARRAY(newIndices, vertexCount);
}


// ---- iterator ----

typedef void (*VertexModificationCallback)( Vertex* vertex );
static void ModifyVertices( MeshBuffer* buffer,
                            VertexModificationCallback cb )
{
    Vertex* vertices      = &buffer->vertices[0];
    const int vertexCount = GetMeshBufferVertexCount(buffer);
    REPEAT(vertexCount, i)
    {
        Vertex* vertex = &vertices[i];
        cb(vertex);
    }
}

typedef void (*TriangleModificationCallback)( Vertex* a, Vertex* b, Vertex* c );
static void ModifyTriangles( MeshBuffer* buffer,
                             TriangleModificationCallback cb )
{
    Vertex* vertices      = &buffer->vertices[0];
    VertexIndex* indices  = &buffer->indices[0];
    const int vertexCount = GetMeshBufferVertexCount(buffer);
    const int indexCount  = GetMeshBufferIndexCount(buffer);

    if(indexCount > 0)
    {
        assert(indexCount % 3 == 0);
        for(int i = 0; i < indexCount; i+=3)
        {
            Vertex* a = &vertices[indices[i+0]];
            Vertex* b = &vertices[indices[i+1]];
            Vertex* c = &vertices[indices[i+2]];
            cb(a,b,c);
        }
    }
    else
    {
        assert(vertexCount % 3 == 0);
        for(int i = 0; i < vertexCount; i+=3)
        {
            Vertex* a = &vertices[i+0];
            Vertex* b = &vertices[i+1];
            Vertex* c = &vertices[i+2];
            cb(a,b,c);
        }
    }
}


// ---- normal calculation ----

static void ResetVertexNormal( Vertex* vertex )
{
    vertex->normal = Vec3Zero;
}

static void NormalizeVertexNormal( Vertex* vertex )
{
    vertex->normal = NormalizeVec3(vertex->normal);
}

static void CalcMeshBufferNormals( MeshBuffer* buffer )
{
    ModifyVertices(buffer, ResetVertexNormal);
    ModifyTriangles(buffer, CalcTriangleNormal);
    ModifyVertices(buffer, NormalizeVertexNormal);
}


// ---- tangent calculation ----

static void ResetVertexTangents( Vertex* vertex )
{
    vertex->tangent   = Vec3Zero;
    vertex->bitangent = Vec3Zero;
}

static void NormalizeVertexTangents( Vertex* vertex )
{
    vertex->tangent   = NormalizeVec3(vertex->tangent);
    vertex->bitangent = NormalizeVec3(vertex->bitangent);
}

static void CalcMeshBufferTangents( MeshBuffer* buffer )
{
    ModifyVertices(buffer, ResetVertexTangents);
    ModifyTriangles(buffer, CalcTriangleTangents);
    ModifyVertices(buffer, NormalizeVertexTangents);
}


// ---- jobs ----

struct MeshBufferPostprocessingJobDesc
{
    MeshBuffer* buffer;
    int options;
};

static void DestroyMeshBufferPostprocessingJob( void* _desc )
{
    MeshBufferPostprocessingJobDesc* desc =
        (MeshBufferPostprocessingJobDesc*)_desc;
    ReleaseMeshBuffer(desc->buffer);
    DELETE(desc);
}

static void ProcessMeshBufferPostprocessingJob( void* _desc )
{
    MeshBufferPostprocessingJobDesc* desc =
        (MeshBufferPostprocessingJobDesc*)_desc;

    if(desc->options & MESH_BUFFER_INDEX)
        IndexMeshBuffer(desc->buffer);

    if(desc->options & MESH_BUFFER_CALC_NORMALS)
        CalcMeshBufferNormals(desc->buffer);

    if(desc->options & MESH_BUFFER_CALC_TANGENTS)
        CalcMeshBufferTangents(desc->buffer);
}

JobId BeginMeshBufferPostprocessing( MeshBuffer* buffer, int options )
{
    MeshBufferPostprocessingJobDesc* desc =
        NEW(MeshBufferPostprocessingJobDesc);
    desc->buffer = buffer;
    desc->options = options;
    ReferenceMeshBuffer(buffer);
    return CreateJob({"PostprocessMeshBuffer",
                      ProcessMeshBufferPostprocessingJob,
                      DestroyMeshBufferPostprocessingJob,
                      desc});
}
