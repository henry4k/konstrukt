#include <string.h> // memset

#include "Common.h"
#include "Mesh.h"
#include "MeshBuffer.h"
#include "Reference.h"
#include "PhysicsManager.h"
#include "ChunkGenerator.h"


static const int MAX_VOXEL_REPRESENTATIONS = 8;

enum VoxelRepresentationType
{
    BLOCK_VOXEL_REPRESENTATION
};

static const int MAX_BLOCK_MESHES = 4;
static const int MAX_BLOCK_COLLISION_SHAPES = 4;


struct BlockVoxelRepresentation
{
    VoxelRepresentationOpeningState openingState;

    BlockVoxelRepresentationMesh meshes[MAX_BLOCK_MESHES];
    int meshCount;

    CollisionShape* collisionShapes[MAX_BLOCK_COLLISION_SHAPES];
    int collisionShapeCount;
};

struct VoxelRepresentation
{
    VoxelRepresentationType type;
    union data_
    {
        BlockVoxelRepresentation block;
    } data;
};

struct ChunkGenerator
{
    ReferenceCounter refCounter;
    VoxelRepresentation voxelRepresentations[MAX_VOXEL_REPRESENTATIONS];
    int voxelRepresentationCount;
};


static void FreeVoxelRepresentation( VoxelRepresentation* representation );
//static void FreeVoxelRepresentationMesh( VoxelRepresentationMesh* mesh );


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

static void* CreateVoxelRepresentation( ChunkGenerator* generator,
                                        VoxelRepresentationType type)
{
    if(generator->voxelRepresentationCount < MAX_VOXEL_REPRESENTATIONS)
    {
        const int i = generator->voxelRepresentationCount;
        VoxelRepresentation* representation =
            &generator->voxelRepresentations[i];
        representation->type = type;
        generator->voxelRepresentationCount++;
        return (void*)&representation->data;
    }
    else
    {
        Error("Can't create more voxel representations.");
        return NULL;
    }
}

bool CreateBlockVoxelRepresentation( ChunkGenerator* generator,
                                     VoxelRepresentationOpeningState state,
                                     BlockVoxelRepresentationMesh* meshes,
                                     int meshCount,
                                     CollisionShape** collisionShapes,
                                     int collisionShapeCount )
{
    if(meshCount > MAX_BLOCK_MESHES)
    {
        Error("Too many meshes for a block voxel representation.");
        return false;
    }

    if(collisionShapeCount > MAX_BLOCK_COLLISION_SHAPES)
    {
        Error("Too many collision shapes for a block voxel representation.");
        return false;
    }

    BlockVoxelRepresentation* representation =
        (BlockVoxelRepresentation*)CreateVoxelRepresentation(generator, BLOCK_VOXEL_REPRESENTATION);
    if(representation)
    {
        representation->openingState = state;

        representation->meshCount = meshCount;
        for(int i = 0; i < meshCount; i++)
        {
            representation->meshes[i] = meshes[i];
            BlockVoxelRepresentationMesh* mesh = &meshes[i];
            for(int i = 0; i < BLOCK_VOXEL_REPRESENTATION_SUB_MESH_COUNT; i++)
                if(mesh->buffers[i])
                    ReferenceMeshBuffer(mesh->buffers[i]);
        }

        representation->collisionShapeCount = collisionShapeCount;
        for(int i = 0; i < collisionShapeCount; i++)
        {
            representation->collisionShapes[i] = collisionShapes[i];
            ReferenceCollisionShape(collisionShapes[i]);
        }

        return true;
    }
    else
    {
        return false;
    }
}

static void FreeBlockVoxelRepresentationMesh( BlockVoxelRepresentationMesh* mesh )
{
    for(int i = 0; i < BLOCK_VOXEL_REPRESENTATION_SUB_MESH_COUNT; i++)
        if(mesh->buffers[i])
            ReleaseMeshBuffer(mesh->buffers[i]);
}

static void FreeBlockVoxelRepresentation( BlockVoxelRepresentation* representation )
{
    for(int i = 0; i < representation->meshCount; i++)
        FreeBlockVoxelRepresentationMesh(&representation->meshes[i]);
    for(int i = 0; i < representation->collisionShapeCount; i++)
        ReleaseCollisionShape(representation->collisionShapes[i]);
}

static void FreeVoxelRepresentation( VoxelRepresentation* representation )
{
    switch(representation->type)
    {
        case BLOCK_VOXEL_REPRESENTATION:
            FreeBlockVoxelRepresentation((BlockVoxelRepresentation*)&representation->data);
            return;
    }
    FatalError("Unknown voxel representation type.");
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
