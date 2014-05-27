#ifndef __MESH_BUFFER__
#define __MESH_BUFFER__

#include <vector>
#include "Vertex.h"
#include "Math.h"

// TODO: Hide MeshBuffer structure and provide a better API instead.

struct MeshBuffer
{
    std::vector<Vertex> vertices;
    std::vector<VertexIndex> indices;
};

void CreateMeshBuffer( MeshBuffer* buffer );
void FreeMeshBuffer( MeshBuffer* buffer );

void TransformMeshBuffer( MeshBuffer* buffer, const glm::mat4* transformation );
void AppendMeshBuffer( MeshBuffer* buffer, const MeshBuffer* otherBuffer, const glm::mat4* transformation );

#endif
