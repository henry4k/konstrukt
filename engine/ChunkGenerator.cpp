#include <string.h> // memset

#include "Common.h"
#include "Mesh.h"
#include "MeshBuffer.h"
#include "Reference.h"
#include "PhysicsManager.h"
#include "ChunkGenerator.h"


static const int MAX_VOXEL_REPRESENTATIONS = 8;
static const int MAX_VOXEL_REPRESENTATION_MESHES = 4;
static const int MAX_VOXEL_REPRESENTATION_COLLISION_SHAPES = 4;


struct VoxelRepresentationMesh
{
    int id;
    MeshBuffer* subMeshBuffers[VOXEL_REPRESENTATION_SUB_MESH_COUNT];
};

struct VoxelRepresentation
{
    VoxelRepresentationOpeningState openingState;

    VoxelRepresentationMesh meshes[MAX_VOXEL_REPRESENTATION_MESHES];
    int meshCount;

    CollisionShape* collisionShapes[MAX_VOXEL_REPRESENTATION_COLLISION_SHAPES];
    int collisionShapeCount;
};

struct ChunkGenerator
{
    ReferenceCounter refCounter;
    VoxelRepresentation voxelRepresentations[MAX_VOXEL_REPRESENTATIONS];
    int voxelRepresentationCount;
};



static void FreeVoxelRepresentation( VoxelRepresentation* representation );
static void FreeVoxelRepresentationMesh( VoxelRepresentationMesh* mesh );


ChunkGenerator* CreateChunkGenerator()
{
    ChunkGenerator* generator = new ChunkGenerator;
    memset(generator, 0, sizeof(ChunkGenerator));
    InitReferenceCounter(&generator->refCounter);
    return generator;
}

static void FreeChunkGenerator( ChunkGenerator* generator )
{
    for(int i = 0; i < generator->voxelRepresentationCount; i++)
    {
        VoxelRepresentation* representation =
            &generator->voxelRepresentations[i];
        FreeVoxelRepresentation(representation);
    }
    delete generator;
}

void ReferenceChunkGenerator( ChunkGenerator* generator )
{
    Reference(&generator->refCounter);
}

void ReleaseChunkGenerator( ChunkGenerator* generator )
{
    Release(&generator->refCounter);
    if(!HasReferences(&generator->refCounter))
        FreeChunkGenerator(generator);
}

Chunk* GenerateChunk( ChunkGenerator* generator,
                      int x, int y, int z,
                      int w, int h, int d )
{
    Chunk* chunk = new Chunk;
    memset(chunk, 0, sizeof(Chunk));

    // TODO

    return chunk;
}

void FreeChunk( Chunk* chunk )
{
    for(int i = 0; i < chunk->meshCount; i++)
        ReleaseMesh(chunk->meshes[i]);
    delete[] chunk->meshes;
    delete[] chunk->meshIds;

    for(int i = 0; i < chunk->collisionShapeCount; i++)
        ReleaseCollisionShape(chunk->collisionShapes[i]);
    delete[] chunk->collisionShapes;

    delete chunk;
}


VoxelRepresentation* CreateVoxelRepresentation( ChunkGenerator* generator )
{
    if(generator->voxelRepresentationCount < MAX_VOXEL_REPRESENTATIONS)
    {
        const int i = generator->voxelRepresentationCount;
        VoxelRepresentation* representation =
            &generator->voxelRepresentations[i];
        generator->voxelRepresentationCount++;
        // memset(representation, 0, sizeof(VoxelRepresentation));
        // ^- Not necessary, as CreateChunkGenerator already clears the memory.
        return representation;
    }
    else
    {
        Error("Can't create more voxel representations.");
        return NULL;
    }
}

static void FreeVoxelRepresentation( VoxelRepresentation* representation )
{
    for(int i = 0; i < representation->meshCount; i++)
        FreeVoxelRepresentationMesh(&representation->meshes[i]);
    for(int i = 0; i < representation->collisionShapeCount; i++)
        ReleaseCollisionShape(representation->collisionShapes[i]);
}

void SetVoxelRepresentationOpeningState( VoxelRepresentation* representation,
                                         VoxelRepresentationOpeningState state )
{
    representation->openingState = state;
}

void AddMeshToVoxelRepresentation( VoxelRepresentation* representation,
                                   int meshId,
                                   MeshBuffer** subMeshBuffers )
{
    if(representation->meshCount < MAX_VOXEL_REPRESENTATION_MESHES)
    {
        const int i = representation->meshCount;
        VoxelRepresentationMesh* mesh = &representation->meshes[i];
        representation->meshCount++;
        // memset(mesh, 0, sizeof(VoxelRepresentationMesh));
        // ^- Not necessary, as CreateChunkGenerator already clears the memory.
        mesh->id = meshId;

        for(int i = 0; i < VOXEL_REPRESENTATION_SUB_MESH_COUNT; i++)
            mesh->subMeshBuffers[i] = subMeshBuffers[i];
    }
    else
    {
        Error("Can't add more meshes to voxel representation.");
    }
}

static void FreeVoxelRepresentationMesh( VoxelRepresentationMesh* mesh )
{
    for(int i = 0; i < VOXEL_REPRESENTATION_SUB_MESH_COUNT; i++)
        if(mesh->subMeshBuffers[i])
            FreeMeshBuffer(mesh->subMeshBuffers[i]);
}

void AddCollisionShapeToVoxelRepresentation( VoxelRepresentation* representation,
                                             CollisionShape* shape )
{
    if(representation->collisionShapeCount < MAX_VOXEL_REPRESENTATION_COLLISION_SHAPES)
    {
        const int i = representation->collisionShapeCount;
        representation->collisionShapes[i] = shape;
        representation->collisionShapeCount++;
        ReferenceCollisionShape(shape);
    }
    else
    {
        Error("Can't add more collision shapes to voxel representation.");
    }
}
