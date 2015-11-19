#include "Common.h"
#include "Mesh.h"
#include "MeshBuffer.h"
#include "Reference.h"
#include "VoxelVolume.h"
#include "MeshChunkGenerator.h"


static const int MAX_VOXEL_MESHES = 8;

enum VoxelMeshType
{
    BLOCK_VOXEL_MESH
};


struct BlockVoxelMesh
{
    VoxelOpeningState openingState;
    int materialId;
    MeshBuffer* meshBuffers[BLOCK_VOXEL_MATERIAL_BUFFER_COUNT];
};

struct VoxelMesh
{
    VoxelMeshType type;
    union data_
    {
        BlockVoxelMesh block;
    } data;
};

struct MeshChunkGenerator
{
    ReferenceCounter refCounter;
    VoxelMesh voxelMeshes[MAX_VOXEL_MESHES];
    int voxelMeshCount;
    BitConditionSolver* blockMeshConditions;
};


static void FreeVoxelMesh( VoxelMesh* mesh );

MeshChunkGenerator* CreateMeshChunkGenerator()
{
    MeshChunkGenerator* generator = new MeshChunkGenerator;
    memset(generator, 0, sizeof(MeshChunkGenerator));
    InitReferenceCounter(&generator->refCounter);
    generator->blockMeshConditions = CreateBitConditionSolver();
    return generator;
}

static void FreeMeshChunkGenerator( MeshChunkGenerator* generator )
{
    for(int i = 0; i < generator->voxelMeshCount; i++)
    {
        VoxelMesh* mesh = &generator->voxelMeshes[i];
        FreeVoxelMesh(mesh);
    }
    FreeBitConditionSolver(generator->blockMeshConditions);
    delete generator;
}

void ReferenceMeshChunkGenerator( MeshChunkGenerator* generator )
{
    Reference(&generator->refCounter);
}

void ReleaseMeshChunkGenerator( MeshChunkGenerator* generator )
{
    Release(&generator->refCounter);
    if(!HasReferences(&generator->refCounter))
        FreeMeshChunkGenerator(generator);
}

static void* CreateVoxelMesh( MeshChunkGenerator* generator,
                              VoxelMeshType type )
{
    if(generator->voxelMeshCount < MAX_VOXEL_MESHES)
    {
        const int i = generator->voxelMeshCount;
        VoxelMesh* mesh = &generator->voxelMeshes[i];
        mesh->type = type;
        generator->voxelMeshCount++;
        return (void*)&mesh->data;
    }
    else
    {
        Error("Can't create more voxel meshes.");
        return NULL;
    }
}



bool CreateBlockVoxelMesh( MeshChunkGenerator* generator,
                           VoxelOpeningState openingState,
                           int materialId,
                           const BitCondition* conditions,
                           int conditionCount,
                           MeshBuffer* meshBuffers[BLOCK_VOXEL_MATERIAL_BUFFER_COUNT] )
{
    BlockVoxelMesh* mesh = (BlockVoxelMesh*)CreateVoxelMesh(generator, BLOCK_VOXEL_MESH);
    if(mesh)
    {
        mesh->openingState = openingState;
        mesh->materialId   = materialId;

        memcpy(mesh->meshBuffers, meshBuffers,
               sizeof(MeshBuffer*) * BLOCK_VOXEL_MATERIAL_BUFFER_COUNT);

        for(int i = 0; i < BLOCK_VOXEL_MATERIAL_BUFFER_COUNT; i++)
            if(mesh->meshBuffers[i])
                ReferenceMeshBuffer(mesh->meshBuffers[i]);

        AddBitConditions(generator->blockMeshConditions,
                         conditions,
                         conditionCount,
                         mesh);

        return true;
    }
    else
    {
        return false;
    }
}

static void FreeBlockVoxelMesh( BlockVoxelMesh* mesh )
{
    for(int i = 0; i < BLOCK_VOXEL_MATERIAL_BUFFER_COUNT; i++)
        ReleaseMeshBuffer(mesh->meshBuffers[i]);
}

static void FreeVoxelMesh( VoxelMesh* mesh )
{
    switch(mesh->type)
    {
        case BLOCK_VOXEL_MESH:
            FreeBlockVoxelMesh((BlockVoxelMesh*)&mesh->data);
            return;
    }
    FatalError("Unknown voxel mesh type.");
}

MeshChunk* GenerateMeshChunk( MeshChunkGenerator* generator,
                              VoxelVolume* volume,
                              int x, int y, int z,
                              int w, int h, int d )
{
    // TODO
    return NULL;
}

void FreeMeshChunk( MeshChunk* chunk )
{
    // TODO
}
