#ifndef __KONSTRUKT_MESH_BUFFER__
#define __KONSTRUKT_MESH_BUFFER__

#include "Vertex.h"
#include "Math.h"
#include "JobManager.h" // JobId

struct MeshBuffer;

enum MeshBufferPostprocessingOptions
{
    MESH_BUFFER_INDEX         = (1 << 0),
    MESH_BUFFER_CALC_NORMALS  = (1 << 1),
    MESH_BUFFER_CALC_TANGENTS = (1 << 2)
};

MeshBuffer* CreateMeshBuffer();

void ReferenceMeshBuffer( MeshBuffer* buffer );
void ReleaseMeshBuffer( MeshBuffer* buffer );

void AddVertexToMeshBuffer( MeshBuffer* buffer, const Vertex* vertex );
void AddIndexToMeshBuffer( MeshBuffer* buffer, VertexIndex index );
void TransformMeshBuffer( MeshBuffer* buffer, Mat4 transformation );
void AppendMeshBuffer( MeshBuffer* buffer, const MeshBuffer* otherBuffer, const Mat4* transformation );

int GetMeshBufferVertexCount( const MeshBuffer* buffer );
const Vertex* GetMeshBufferVertices( const MeshBuffer* buffer );
int GetMeshBufferIndexCount( const MeshBuffer* buffer );
const VertexIndex* GetMeshBufferIndices( const MeshBuffer* buffer );

/**
 * Starts a job which will enrich the mesh buffer according to the selected
 * `options`.
 *
 * Note that the mesh buffer *mustn't* be used while the job runs!
 *
 * @see MeshBufferPostprocessingOptions
 */
JobId BeginMeshBufferPostprocessing( MeshBuffer* buffer, int options );

#endif
