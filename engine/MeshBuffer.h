#ifndef __APOAPSIS_MESH_BUFFER__
#define __APOAPSIS_MESH_BUFFER__

#include "Vertex.h"
#include "Math.h"

struct MeshBuffer;

MeshBuffer* CreateMeshBuffer();

void ReferenceMeshBuffer( MeshBuffer* buffer );
void ReleaseMeshBuffer( MeshBuffer* buffer );

void AddVertexToMeshBuffer( MeshBuffer* buffer, const Vertex* vertex );
void AddIndexToMeshBuffer( MeshBuffer* buffer, VertexIndex index );
void TransformMeshBuffer( MeshBuffer* buffer, Mat4 transformation );
void AppendMeshBuffer( MeshBuffer* buffer, const MeshBuffer* otherBuffer, const Mat4* transformation );
void IndexMeshBuffer( MeshBuffer* buffer );
void CalcMeshBufferNormals( MeshBuffer* buffer );
void CalcMeshBufferTangents( MeshBuffer* buffer );

int GetMeshBufferVertexCount( const MeshBuffer* buffer );
const Vertex* GetMeshBufferVertices( const MeshBuffer* buffer );
int GetMeshBufferIndexCount( const MeshBuffer* buffer );
const VertexIndex* GetMeshBufferIndices( const MeshBuffer* buffer );

#endif
