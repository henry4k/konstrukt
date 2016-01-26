#ifndef __APOAPSIS_MESH_CHUNK_GENERATOR__
#define __APOAPSIS_MESH_CHUNK_GENERATOR__

#include "Math.h"
#include "BitCondition.h"


struct VoxelVolume;
struct MeshBuffer;
struct Mesh;


enum BlockVoxelMeshBuffers
{
    CENTER, /** Special buffer, which is used if any cube side is visible. */
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
 * The chunk generator uses voxel meshes to determine what geometry
 * each voxel needs.
 */
struct MeshChunkGenerator;

/**
 * Result returned by #GenerateMeshChunk.
 *
 * Contains a mesh for each distinct material id encountered during
 * generation.
 *
 * Use #FreeMeshChunk to release the structure, when you don't need it
 * anymore.
 */
struct MeshChunk
{
    Mesh** materialMeshes;
    int*   materialIds;
    int    materialCount;
};


MeshChunkGenerator* CreateMeshChunkGenerator();
void ReferenceMeshChunkGenerator( MeshChunkGenerator* generator );
void ReleaseMeshChunkGenerator( MeshChunkGenerator* generator );

/**
 * A voxel mesh, which defines geometry for each side of a cube.
 *
 * @param materialId
 * #GenerateMeshChunk uses this id to determine the mesh in which the voxels
 * geometry should be written.
 *
 * @param conditions
 * Geometry is only generated for voxels, which match these conditions.
 *
 * @param transparent
 * Whether the geometry is rendered transparent later or if it has holes,
 * through which one can look behind the voxel.
 *
 * @param meshBuffers
 * A mesh buffer for each cube side defined in #BlockVoxelMeshBuffers.
 *
 * @param transformations
 * A transformation which is applied to each cube side defined in
 * #BlockVoxelMeshBuffers.
 *
 * @return
 * Whether the voxel mesh has been created successfully.
 */
bool CreateBlockVoxelMesh( MeshChunkGenerator* generator,
                           int materialId,
                           const BitCondition* conditions,
                           int conditionCount,
                           bool transparent,
                           MeshBuffer** meshBuffers,
                           const Mat4* transformations);

/**
 * Generates a #MeshChunk from a section of a voxel volume.
 */
MeshChunk* GenerateMeshChunk( MeshChunkGenerator* generator,
                              VoxelVolume* volume,
                              int x, int y, int z,
                              int w, int h, int d );

void FreeMeshChunk( MeshChunk* chunk );

#endif
