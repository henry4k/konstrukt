#ifndef __APOAPSIS_CHUNK_GENERATOR__
#define __APOAPSIS_CHUNK_GENERATOR__

/* --- TODO ---

Voxel-Typ-ID in VoxelRepresentation speichern, damit der Generator weiss
welche VoxelRepresentation zuständig ist.

VoxelRepresentation nutzt erste N Bits des Voxels für Typ-spezifische Daten.
*/

struct Mesh;
struct MeshBuffer;
struct CollisionShape;


enum VoxelRepresentationOpeningState
{
    VOXEL_REPRESENTATION_CLOSED,
    VOXEL_REPRESENTATION_TRANSPARENT,
    VOXEL_REPRESENTATION_OPEN
};

enum BlockVoxelRepresentationMaterialBuffers
{
    CENTER,
    POSITIVE_X,
    NEGATIVE_X,
    POSITIVE_Y,
    NEGATIVE_Y,
    POSITIVE_Z,
    NEGATIVE_Z,
    BLOCK_VOXEL_REPRESENTATION_MATERIAL_BUFFER_COUNT
};


/**
 * Generates a graphical and physical representation of a voxel set.
 *
 * The chunk generator uses voxel representations to determine how
 * geometry and collision shapes should be created for each voxel.
 */
struct ChunkGenerator;

/**
 * Is spat out by #GenerateChunk.
 */
struct Chunk
{
    Mesh** materialMeshes;
    int* materialIds;
    int materialCount;

    CollisionShape** collisionShapes;
    int collisionShapeCount;
};

struct BlockVoxelRepresentationMaterial
{
    int id;
    MeshBuffer* buffers[BLOCK_VOXEL_REPRESENTATION_MATERIAL_BUFFER_COUNT];
};


ChunkGenerator* CreateChunkGenerator();

void ReferenceChunkGenerator( ChunkGenerator* generator );
void ReleaseChunkGenerator( ChunkGenerator* generator );

bool CreateBlockVoxelRepresentation( ChunkGenerator* generator,
                                     int voxelType,
                                     VoxelRepresentationOpeningState state,
                                     BlockVoxelRepresentationMaterial* materials,
                                     int materialCount,
                                     CollisionShape** collisionShapes,
                                     int collisionShapeCount );

Chunk* GenerateChunk( ChunkGenerator* generator,
                      int x, int y, int z,
                      int w, int h, int d );

void FreeChunk( Chunk* chunk );

#endif
