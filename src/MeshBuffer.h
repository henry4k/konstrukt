#ifndef __MESH_BUFFER__
#define __MESH_BUFFER__

#include <vector>
#include "Vertex.h"
#include "Math.h"
#include "Mesh.h"

struct MeshBuffer
{
    std::vector<Vertex> vertices;
    std::vector<unsigned short> indices;
};

void CreateMeshBuffer( MeshBuffer* buffer );
void FreeMeshBuffer( MeshBuffer* buffer );
void BuildMesh( const MeshBuffer* buffer, Mesh* meshOut );

void TransformMeshBuffer( MeshBuffer* buffer, const glm::mat4* transformation );
void AddMeshToMeshBuffer( MeshBuffer* buffer, const Mesh* mesh, const glm::mat4* transformation );
void AddMeshBufferToMeshBuffer( MeshBuffer* buffer, const MeshBuffer* mesh, const glm::mat4* transformation );


#endif
