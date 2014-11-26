#include <vector>

#include "Common.h"
#include "Math.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "MeshBuffer.h"

using namespace glm;


struct MeshBuffer
{
    std::vector<Vertex> vertices;
    std::vector<VertexIndex> indices;
};


MeshBuffer* CreateMeshBuffer()
{
    return new MeshBuffer;
}

void FreeMeshBuffer( MeshBuffer* buffer )
{
    delete buffer;
}

void AddVertexToMeshBuffer( MeshBuffer* buffer, const Vertex* vertex )
{
    buffer->vertices.push_back(*vertex);
}

void AddIndexToMeshBuffer( MeshBuffer* buffer, VertexIndex index )
{
    buffer->indices.push_back(index);
}

static void TransformMeshBufferRange( MeshBuffer* buffer, const glm::mat4* transformation, int firstVertex, int vertexCount )
{
    using namespace glm;

    assert(firstVertex >= 0);
    assert(vertexCount >= 0);
    assert(firstVertex+vertexCount <= buffer->vertices.size());

    const mat3 rotation(*transformation);

    Vertex* vertex = &buffer->vertices[firstVertex];
    const Vertex* end = &buffer->vertices[firstVertex+vertexCount];
    for(; vertex != end; ++vertex)
    {
        vertex->position = vec3( *transformation * vec4(vertex->position, 1) );
        vertex->normal   = normalize(rotation * vertex->normal);
        vertex->tangent  = *transformation * vertex->tangent;
    }
}

void TransformMeshBuffer( MeshBuffer* buffer, const glm::mat4* transformation )
{
    TransformMeshBufferRange(buffer, transformation, 0, buffer->vertices.size());
}

void AppendMeshBuffer( MeshBuffer* buffer, const MeshBuffer* otherBuffer, const glm::mat4* transformation )
{
    // TODO: Raise error if target buffer doesn't use indices, but source buffer does.
    // TODO: Generate indices if target buffer uses them, but source buffer doesn't.

    buffer->indices.reserve(buffer->indices.size()+otherBuffer->indices.size());
    const VertexIndex indexOffset = buffer->vertices.size();
    for(const VertexIndex& index : otherBuffer->indices)
        buffer->indices.push_back(index+indexOffset);

    const int start = buffer->vertices.size();
    buffer->vertices.insert(
        buffer->vertices.end(),
        otherBuffer->vertices.begin(),
        otherBuffer->vertices.end()
    );
    if(transformation)
        TransformMeshBufferRange(buffer, transformation, start, otherBuffer->vertices.size());
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
