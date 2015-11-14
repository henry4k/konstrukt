#ifndef __APOAPSIS_CHUNK_GENERATOR__
#define __APOAPSIS_CHUNK_GENERATOR__

struct Mesh;
struct MeshBuffer;
struct CollisionShape;


enum VoxelRepresentationOpeningState
{
    VOXEL_REPRESENTATION_CLOSED,
    VOXEL_REPRESENTATION_TRANSPARENT,
    VOXEL_REPRESENTATION_OPEN
};

enum VoxelRepresentationSubMesh
{
    CENTER,
    POSITIVE_X,
    NEGATIVE_X,
    POSITIVE_Y,
    NEGATIVE_Y,
    POSITIVE_Z,
    NEGATIVE_Z,
    VOXEL_REPRESENTATION_SUB_MESH_COUNT
};


struct ChunkGenerator;
struct VoxelRepresentation;

struct Chunk
{
    Mesh** meshes;
    int* meshIds;
    int meshCount;

    CollisionShape** collisionShapes;
    int collisionShapeCount;
};


ChunkGenerator* CreateChunkGenerator();

void ReferenceChunkGenerator( ChunkGenerator* generator );
void ReleaseChunkGenerator( ChunkGenerator* generator );

Chunk* GenerateChunk( ChunkGenerator* generator,
                      int x, int y, int z,
                      int w, int h, int d );

void FreeChunk( Chunk* chunk );


VoxelRepresentation* CreateVoxelRepresentation( ChunkGenerator* generator );

void SetVoxelRepresentationOpeningState( VoxelRepresentation* representation,
                                         VoxelRepresentationOpeningState state );

void AddMeshToVoxelRepresentation( VoxelRepresentation* representation,
                                   int meshId,
                                   MeshBuffer** subMeshBuffers );

void AddCollisionShapeToVoxelRepresentation( VoxelRepresentation* representation,
                                             CollisionShape* shape );

#endif
