#ifndef __APOAPSIS_MESH_CHUNK_GENERATOR__
#define __APOAPSIS_MESH_CHUNK_GENERATOR__

#include "ChunkGeneratorShared.h"


struct VoxelVolume;
struct MeshBuffer;
struct Mesh;


enum BlockVoxelMeshBuffers
{
    CENTER,
    POSITIVE_X,
    NEGATIVE_X,
    POSITIVE_Y,
    NEGATIVE_Y,
    POSITIVE_Z,
    NEGATIVE_Z,
    BLOCK_VOXEL_MATERIAL_BUFFER_COUNT
};


/**
 * Generates a graphical representation of a voxel set.
 *
 * The chunk generator uses voxel representations to determine what geometry
 * each voxel needs.
 */
struct MeshChunkGenerator;

struct MeshChunk
{
    Mesh** materialMeshes;
    int*   materialIds;
    int    materialCount;
};


MeshChunkGenerator* CreateMeshChunkGenerator();
void ReferenceMeshChunkGenerator( MeshChunkGenerator* generator );
void ReleaseMeshChunkGenerator( MeshChunkGenerator* generator );

bool CreateBlockVoxelMesh( MeshChunkGenerator* generator,
                           VoxelOpeningState openingState,
                           const BitCondition* conditions,
                           int conditionCount,
                           int materialId,
                           MeshBuffer* meshBuffers[BLOCK_VOXEL_MATERIAL_BUFFER_COUNT] );

MeshChunk* GenerateMeshChunk( MeshChunkGenerator* generator,
                              VoxelVolume* volume,
                              int x, int y, int z,
                              int w, int h, int d );

void FreeMeshChunk( MeshChunk* chunk );

#endif
