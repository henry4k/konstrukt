#include <assert.h>
#include <string.h> // memset

#include "Common.h"
#include "Array.h"
#include "Math.h"
#include "Mesh.h"
#include "MeshBuffer.h"
#include "Reference.h"
#include "VoxelVolume.h"
#include "MeshChunkGenerator.h"


static const int MAX_VOXEL_MESHES = 64;

enum VoxelMeshType
{
    BLOCK_VOXEL_MESH
};

enum Neighborhood
{
    NEIGHBOR_POSITIVE_X = 1<<0,
    NEIGHBOR_NEGATIVE_X = 1<<1,
    NEIGHBOR_POSITIVE_Y = 1<<2,
    NEIGHBOR_NEGATIVE_Y = 1<<3,
    NEIGHBOR_POSITIVE_Z = 1<<4,
    NEIGHBOR_NEGATIVE_Z = 1<<5
};


struct BlockVoxelMesh
{
    bool transparent;
    MeshBuffer* meshBuffers[BLOCK_VOXEL_MATERIAL_BUFFER_COUNT];
    Mat4    transformations[BLOCK_VOXEL_MATERIAL_BUFFER_COUNT];
};

struct VoxelMesh
{
    VoxelMeshType type;
    int materialId;
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
    BitConditionSolver* meshConditions;
};

typedef Array<VoxelMesh*> VoxelMeshList;

struct MaterialMeshBuffer
{
    int materialId;
    MeshBuffer* meshBuffer;
};

struct ChunkEnvironment
{
    int w, h, d;

    // Each of these arrays has `w*h*d` elements:
    Voxel*         voxels;
    VoxelMeshList* meshLists;
    char*          transparentVoxels;
    int*           transparentNeighbors;

    Array<MaterialMeshBuffer> materialMeshBuffers;
};


static void DestroyVoxelMesh( VoxelMesh* mesh );

MeshChunkGenerator* CreateMeshChunkGenerator()
{
    MeshChunkGenerator* generator = NEW(MeshChunkGenerator);
    InitReferenceCounter(&generator->refCounter);
    generator->meshConditions = CreateBitConditionSolver();
    return generator;
}

static void FreeMeshChunkGenerator( MeshChunkGenerator* generator )
{
    REPEAT(generator->voxelMeshCount, i)
    {
        VoxelMesh* mesh = &generator->voxelMeshes[i];
        DestroyVoxelMesh(mesh);
    }
    FreeBitConditionSolver(generator->meshConditions);
    DELETE(generator);
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
                              VoxelMeshType type,
                              int materialId,
                              const BitCondition* conditions,
                              int conditionCount )
{
    if(generator->voxelMeshCount >= MAX_VOXEL_MESHES)
        FatalError("Can't create more voxel meshes.");

    const int i = generator->voxelMeshCount;
    VoxelMesh* mesh = &generator->voxelMeshes[i];
    mesh->type = type;
    mesh->materialId = materialId;
    AddBitConditions(generator->meshConditions,
                     conditions,
                     conditionCount,
                     mesh);
    generator->voxelMeshCount++;
    return (void*)&mesh->data;
}

void CreateBlockVoxelMesh( MeshChunkGenerator* generator,
                           int materialId,
                           const BitCondition* conditions,
                           int conditionCount,
                           bool transparent,
                           MeshBuffer** meshBuffers,
                           const Mat4* transformations)
{
    BlockVoxelMesh* mesh = (BlockVoxelMesh*)CreateVoxelMesh(generator,
                                                            BLOCK_VOXEL_MESH,
                                                            materialId,
                                                            conditions,
                                                            conditionCount);
    mesh->transparent = transparent;

    memcpy(mesh->meshBuffers, meshBuffers,
            sizeof(MeshBuffer*) * BLOCK_VOXEL_MATERIAL_BUFFER_COUNT);

    memcpy(mesh->transformations, transformations,
            sizeof(Mat4) * BLOCK_VOXEL_MATERIAL_BUFFER_COUNT);

    REPEAT(BLOCK_VOXEL_MATERIAL_BUFFER_COUNT, i)
        if(mesh->meshBuffers[i])
            ReferenceMeshBuffer(mesh->meshBuffers[i]);
}

static void DestroyBlockVoxelMesh( BlockVoxelMesh* mesh )
{
    REPEAT(BLOCK_VOXEL_MATERIAL_BUFFER_COUNT, i)
        ReleaseMeshBuffer(mesh->meshBuffers[i]);
}

static void DestroyVoxelMesh( VoxelMesh* mesh )
{
    switch(mesh->type)
    {
        case BLOCK_VOXEL_MESH:
            DestroyBlockVoxelMesh((BlockVoxelMesh*)&mesh->data);
            return;
    }
    FatalError("Unknown voxel mesh type.");
}

static bool IsVoxelMeshTransparent( const VoxelMesh* mesh )
{
    switch(mesh->type)
    {
        case BLOCK_VOXEL_MESH:
            return ((BlockVoxelMesh*)&mesh->data)->transparent;
    }
    FatalError("Unknown voxel mesh type.");
    return false;
}

// ----------------------------------------------------------------------

static int Get3DArrayIndex( int x, int y, int z,
                            int w, int h, int d )
{
    assert(x >= 0 &&
           y >= 0 &&
           z >= 0 &&
           x < w &&
           y < h &&
           z < d);
    return z*h*w + y*w + x;
}

static Voxel* ReadVoxelChunk( VoxelVolume* volume,
                              int sx, int sy, int sz,
                              int w, int h, int d )
{
    Voxel* voxels = (Voxel*)AllocZeroed(sizeof(Voxel)*w*h*d);
    REPEAT(d,z)
    REPEAT(h,y)
    REPEAT(w,x)
        ReadVoxelData(volume, sx+x, sy+y, sz+z, &voxels[Get3DArrayIndex(x,y,z,w,h,d)]);
    return voxels;
}

static void ReadVoxelMeshList( const MeshChunkGenerator* generator,
                               const Voxel* voxel,
                               VoxelMeshList* meshList )
{
    BitFieldPayloadList payloadList =
        GatherPayloadFromBitField(generator->meshConditions,
                                  voxel,
                                  sizeof(Voxel));

    if(payloadList.length > 0)
        AppendToArray(meshList, payloadList.length, (VoxelMesh**)payloadList.data);

    DestroyBitFieldPayloadList(&payloadList);
}

static VoxelMeshList* GatherVoxelMeshListsForChunk( const MeshChunkGenerator* generator,
                                                    const Voxel* voxels,
                                                    int w, int h, int d )
{
    const int voxelCount = w*h*d;

    VoxelMeshList* meshLists =
        (VoxelMeshList*)Alloc(sizeof(VoxelMeshList) * voxelCount);
    REPEAT(voxelCount, i)
    {
        InitArray(meshLists + i);
        ReadVoxelMeshList(generator, &voxels[i], meshLists + i);
    }
    return meshLists;
}

static char* GatherTransparentVoxelsForChunk( const VoxelMeshList* meshLists,
                                              int w, int h, int d )
{
    const int voxelCount = w*h*d;
    char* transparentVoxels = (char*)Alloc(voxelCount);
    REPEAT(voxelCount, i)
    {
        const VoxelMeshList* meshList = meshLists + i;
        bool transparent = true;
        REPEAT(meshList->length, j)
        {
            const VoxelMesh* mesh = meshList->data[j];
            if(!IsVoxelMeshTransparent(mesh))
            {
                transparent = false;
                break;
            }
        }
        transparentVoxels[i] = transparent;
    }
    return transparentVoxels;
}

static int GetTransparentNeighborhood( const char* transparentVoxels,
                                       int x, int y, int z,
                                       int w, int h, int d )
{
    assert(x >= 1 && x <= w-2);
    assert(y >= 1 && y <= h-2);
    assert(z >= 1 && z <= d-2);
    int transparentNeighbors = 0;
#define TEST(xo, yo, zo, flag) \
    if(transparentVoxels[Get3DArrayIndex(x+(xo),y+(yo),z+(zo),w,h,d)]) \
        transparentNeighbors |= (flag);
    TEST(+1,0,0,NEIGHBOR_POSITIVE_X)
    TEST(-1,0,0,NEIGHBOR_NEGATIVE_X)
    TEST(0,+1,0,NEIGHBOR_POSITIVE_Y)
    TEST(0,-1,0,NEIGHBOR_NEGATIVE_Y)
    TEST(0,0,+1,NEIGHBOR_POSITIVE_Z)
    TEST(0,0,-1,NEIGHBOR_NEGATIVE_Z)
#undef TEST
    return transparentNeighbors;
}

static int* GatherTransparentNeighborsForChunk( const char* transparentVoxels,
                                                 int w, int h, int d )
{
    int* transparentNeighbors = (int*)AllocZeroed(sizeof(int)*w*h*d);
    for(int z = 1; z < d-1; z++)
    for(int y = 1; y < h-1; y++)
    for(int x = 1; x < w-1; x++)
        transparentNeighbors[Get3DArrayIndex(x,y,z,w,h,d)] =
            GetTransparentNeighborhood(transparentVoxels,
                                       x, y, z,
                                       w, h, d);
    return transparentNeighbors;
}

static MeshBuffer* GetMeshBufferForMaterial( ChunkEnvironment* env,
                                             int materialId )
{
    Array<MaterialMeshBuffer>* buffers = &env->materialMeshBuffers;

    // Look if the material has already been used:
    REPEAT(buffers->length, i)
        if(buffers->data[i].materialId == materialId)
            return buffers->data[i].meshBuffer;

    // Create a new entry:
    MaterialMeshBuffer* buffer = AllocateAtEndOfArray(buffers, 1);
    buffer->materialId = materialId;
    buffer->meshBuffer = CreateMeshBuffer();
    ReferenceMeshBuffer(buffer->meshBuffer);

    return buffer->meshBuffer;
}

static void ProcessBlockVoxelMesh( ChunkEnvironment* env,
                                   int transparentNeighbors,
                                   MeshBuffer* materialMeshBuffer,
                                   float x, float y, float z,
                                   const BlockVoxelMesh* mesh )
{
    static const int dirCount = 6;
    static const int neighborDirs[dirCount] =
    {
        NEIGHBOR_POSITIVE_X,
        NEIGHBOR_NEGATIVE_X,
        NEIGHBOR_POSITIVE_Y,
        NEIGHBOR_NEGATIVE_Y,
        NEIGHBOR_POSITIVE_Z,
        NEIGHBOR_NEGATIVE_Z
    };
    static const BlockVoxelMeshBuffers dirs[dirCount] =
    {
        POSITIVE_X,
        NEGATIVE_X,
        POSITIVE_Y,
        NEGATIVE_Y,
        POSITIVE_Z,
        NEGATIVE_Z
    };

    const MeshBuffer* const* meshBuffers = &mesh->meshBuffers[0];
    const Mat4* transformations = mesh->transformations;
    const Vec3 translationVector = {{x,y,z}};
    const Mat4 voxelTransformation = TranslateMat4(Mat4Identity,
                                                   translationVector);

    if(transparentNeighbors != 0 &&
       meshBuffers[CENTER])
    {
        const Mat4 transformation = MulMat4(voxelTransformation,
                                            transformations[CENTER]);
        AppendMeshBuffer(materialMeshBuffer,
                         meshBuffers[CENTER],
                         &transformation);
    }

    REPEAT(dirCount, i)
    {
        if(transparentNeighbors & neighborDirs[i] && meshBuffers[dirs[i]])
        {
            const Mat4 transformation = MulMat4(voxelTransformation,
                                                transformations[dirs[i]]);
            AppendMeshBuffer(materialMeshBuffer,
                             meshBuffers[dirs[i]],
                             &transformation);
        }
    }
}

static void ProcessVoxelMesh( ChunkEnvironment* env,
                              int transparentNeighbors,
                              float x, float y, float z,
                              const VoxelMesh* mesh )
{
    MeshBuffer* materialMeshBuffer =
        GetMeshBufferForMaterial(env, mesh->materialId);
    switch(mesh->type)
    {
        case BLOCK_VOXEL_MESH:
            ProcessBlockVoxelMesh(env,
                                  transparentNeighbors,
                                  materialMeshBuffer,
                                  x, y, z,
                                  (BlockVoxelMesh*)&mesh->data);
            return;
    }
    FatalError("Unknown voxel mesh type.");
}

static void ProcessVoxelMeshes( ChunkEnvironment* env )
{
    const int w = env->w-2;
    const int h = env->h-2;
    const int d = env->d-2;

    const float hw = ((float)w) / 2.f;
    const float hh = ((float)h) / 2.f;
    const float hd = ((float)d) / 2.f;

    REPEAT(d,z)
    REPEAT(h,y)
    REPEAT(w,x)
    {
        const int envIndex = Get3DArrayIndex(x+1,
                                             y+1,
                                             z+1,
                                             env->w,
                                             env->h,
                                             env->d);
        const int transparentNeighbors = env->transparentNeighbors[envIndex];
        VoxelMeshList* meshList = env->meshLists + envIndex;

        REPEAT(meshList->length, i)
        {
            const VoxelMesh* mesh = meshList->data[i];
            const float xTranslation = ((float)x) - hw + 0.5f;
            const float yTranslation = ((float)y) - hh + 0.5f;
            const float zTranslation = ((float)z) - hd + 0.5f;
            ProcessVoxelMesh(env,
                             transparentNeighbors,
                             xTranslation,
                             yTranslation,
                             zTranslation,
                             mesh);
        }
    }
}

static ChunkEnvironment* CreateChunkEnvironment( MeshChunkGenerator* generator,
                                                 VoxelVolume* volume,
                                                 int sx, int sy, int sz,
                                                 int w, int h, int d )
{
    ChunkEnvironment* env = NEW(ChunkEnvironment);

    env->w = w;
    env->h = h;
    env->d = d;

    env->voxels =
        ReadVoxelChunk(volume, sx, sy, sz, w, h, d);

    env->meshLists =
        GatherVoxelMeshListsForChunk(generator, env->voxels, w, h, d);

    env->transparentVoxels =
        GatherTransparentVoxelsForChunk(env->meshLists, w, h, d);

    env->transparentNeighbors =
        GatherTransparentNeighborsForChunk(env->transparentVoxels, w, h, d);

    ProcessVoxelMeshes(env);
    return env;
}

static void FreeChunkEnvironment( ChunkEnvironment* env )
{
    const int voxelCount = env->w *
                           env->h *
                           env->d;

    REPEAT(voxelCount, i)
        DestroyArray(env->meshLists + i);

    Free(env->voxels);
    Free(env->meshLists);
    Free(env->transparentVoxels);
    Free(env->transparentNeighbors);


    REPEAT(env->materialMeshBuffers.length, i)
        ReleaseMeshBuffer(env->materialMeshBuffers.data[i].meshBuffer);
    DestroyArray(&env->materialMeshBuffers);

    DELETE(env);
}

// ----------------------------------------------------------------------

static MeshChunk* GenerateMeshChunkWithEnv( ChunkEnvironment* env )
{
    MeshChunk* chunk = NEW(MeshChunk);

    const Array<MaterialMeshBuffer>* buffers = &env->materialMeshBuffers;

    chunk->materialCount    = buffers->length;
    chunk->materialIds      =   (int*)Alloc(sizeof(int)   * buffers->length);
    chunk->materialMeshes   = (Mesh**)Alloc(sizeof(Mesh*) * buffers->length);

    REPEAT(buffers->length, i)
    {
        const MaterialMeshBuffer* buffer = buffers->data + i;
        Mesh* mesh = CreateMesh(buffer->meshBuffer);
        ReferenceMesh(mesh);
        chunk->materialMeshes[i] = mesh;
        chunk->materialIds[i] = buffer->materialId;
    }

    return chunk;
}

MeshChunk* GenerateMeshChunk( MeshChunkGenerator* generator,
                              VoxelVolume* volume,
                              int sx, int sy, int sz,
                              int w, int h, int d )
{
    ChunkEnvironment* env = CreateChunkEnvironment(generator,
                                                   volume,
                                                   sx-1, sy-1, sz-1,
                                                    w+2,  h+2,  d+2);
    // ^- Enlarge chunk environment by one.

    MeshChunk* chunk = GenerateMeshChunkWithEnv(env);

    FreeChunkEnvironment(env);
    return chunk;
}

void FreeMeshChunk( MeshChunk* chunk )
{
    REPEAT(chunk->materialCount, i)
        ReleaseMesh(chunk->materialMeshes[i]);
    Free(chunk->materialMeshes);
    Free(chunk->materialIds);
    DELETE(chunk);
}
