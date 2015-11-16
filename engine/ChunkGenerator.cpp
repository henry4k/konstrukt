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

static const int MAX_BLOCK_MATERIALS = 4;
static const int MAX_BLOCK_COLLISION_SHAPES = 4;


struct BlockVoxelRepresentation
{
    VoxelRepresentationOpeningState openingState;

    BlockVoxelRepresentationMaterial materials[MAX_BLOCK_MATERIALS];
    int materialCount;

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
                                     BlockVoxelRepresentationMaterial* materials,
                                     int materialCount,
                                     CollisionShape** collisionShapes,
                                     int collisionShapeCount )
{
    if(materialCount > MAX_BLOCK_MATERIALS)
    {
        Error("Too many materials for a block voxel representation.");
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

        representation->materialCount = materialCount;
        for(int i = 0; i < materialCount; i++)
        {
            representation->materials[i] = materials[i];
            BlockVoxelRepresentationMaterial* material = &materials[i];
            for(int i = 0; i < BLOCK_VOXEL_REPRESENTATION_MATERIAL_BUFFER_COUNT; i++)
                if(material->buffers[i])
                    ReferenceMeshBuffer(material->buffers[i]);
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

static void FreeBlockVoxelRepresentationMaterial( BlockVoxelRepresentationMaterial* material )
{
    for(int i = 0; i < BLOCK_VOXEL_REPRESENTATION_MATERIAL_BUFFER_COUNT; i++)
        if(material->buffers[i])
            ReleaseMeshBuffer(material->buffers[i]);
}

static void FreeBlockVoxelRepresentation( BlockVoxelRepresentation* representation )
{
    for(int i = 0; i < representation->materialCount; i++)
        FreeBlockVoxelRepresentationMaterial(&representation->materials[i]);
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
    for(int i = 0; i < chunk->materialCount; i++)
        ReleaseMesh(chunk->materialMeshes[i]);
    delete[] chunk->materialMeshes;
    delete[] chunk->materialIds;

    for(int i = 0; i < chunk->collisionShapeCount; i++)
        ReleaseCollisionShape(chunk->collisionShapes[i]);
    delete[] chunk->collisionShapes;

    delete chunk;
}
