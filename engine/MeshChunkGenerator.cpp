#include <string.h> // memset
#include <stdlib.h> // realloc, free

#include "Common.h"
#include "Math.h"
#include "Mesh.h"
#include "MeshBuffer.h"
#include "Reference.h"
#include "VoxelVolume.h"
#include "MeshChunkGenerator.h"

#include <glm/gtc/matrix_transform.hpp> // translate, rotate


static const int MAX_VOXEL_MESHES = 8;

enum VoxelMeshType
{
    BLOCK_VOXEL_MESH
};

enum MooreNeighbourhood
{
    MOORE_POSITIVE_X = 1<<0,
    MOORE_NEGATIVE_X = 1<<1,
    MOORE_POSITIVE_Y = 1<<2,
    MOORE_NEGATIVE_Y = 1<<3,
    MOORE_POSITIVE_Z = 1<<4,
    MOORE_NEGATIVE_Z = 1<<5
};


struct BlockVoxelMesh
{
    bool transparent;
    MeshBuffer* meshBuffers[BLOCK_VOXEL_MATERIAL_BUFFER_COUNT];
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

struct VoxelMeshList
{
    VoxelMesh* meshes;
    int meshCount;
};

struct ChunkEnvironment
{
    int w, h, d;
    const Voxel* voxels;
    const VoxelMeshList* meshLists;
    const bool* transparentVoxels;
    const int*  transparentNeighbours;

    MeshBuffer** materialMeshBuffers;
    int*         materialIds;
    int          materialCount;
};


static void FreeVoxelMesh( VoxelMesh* mesh );

MeshChunkGenerator* CreateMeshChunkGenerator()
{
    MeshChunkGenerator* generator = new MeshChunkGenerator;
    memset(generator, 0, sizeof(MeshChunkGenerator));
    InitReferenceCounter(&generator->refCounter);
    generator->meshConditions = CreateBitConditionSolver();
    return generator;
}

static void FreeMeshChunkGenerator( MeshChunkGenerator* generator )
{
    for(int i = 0; i < generator->voxelMeshCount; i++)
    {
        VoxelMesh* mesh = &generator->voxelMeshes[i];
        FreeVoxelMesh(mesh);
    }
    FreeBitConditionSolver(generator->meshConditions);
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
                              VoxelMeshType type,
                              int materialId,
                              const BitCondition* conditions,
                              int conditionCount )
{
    if(generator->voxelMeshCount < MAX_VOXEL_MESHES)
    {
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
    else
    {
        Error("Can't create more voxel meshes.");
        return NULL;
    }
}

bool CreateBlockVoxelMesh( MeshChunkGenerator* generator,
                           int materialId,
                           const BitCondition* conditions,
                           int conditionCount,
                           bool transparent,
                           MeshBuffer* meshBuffers[BLOCK_VOXEL_MATERIAL_BUFFER_COUNT] )
{
    BlockVoxelMesh* mesh = (BlockVoxelMesh*)CreateVoxelMesh(generator,
                                                            BLOCK_VOXEL_MESH,
                                                            materialId,
                                                            conditions,
                                                            conditionCount);
    if(mesh)
    {
        mesh->transparent = transparent;

        memcpy(mesh->meshBuffers, meshBuffers,
               sizeof(MeshBuffer*) * BLOCK_VOXEL_MATERIAL_BUFFER_COUNT);

        for(int i = 0; i < BLOCK_VOXEL_MATERIAL_BUFFER_COUNT; i++)
            if(mesh->meshBuffers[i])
                ReferenceMeshBuffer(mesh->meshBuffers[i]);

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

static bool IsVoxelMeshTransparent( const VoxelMesh* mesh )
{
    switch(mesh->type)
    {
        case BLOCK_VOXEL_MESH:
            return ((BlockVoxelMesh*)&mesh->data)->transparent;
    }
    FatalError("Unknown voxel mesh type.");
}

// ----------------------------------------------------------------------

static int Get3DArrayIndex( int x, int y, int z,
                            int w, int h )
{
    return z*h*w + y*w + x;
}

static Voxel* ReadVoxelChunk( VoxelVolume* volume,
                              int sx, int sy, int sz,
                              int w, int h, int d )
{
    const int voxelCount = w*h*d;
    Voxel* voxels = new Voxel[voxelCount];
    memset(voxels, 0, sizeof(Voxel)*voxelCount);

    const int ex = sx+w;
    const int ey = sy+h;
    const int ez = sz+d;

    for(int z = sz; z < ez; z++)
    for(int y = sy; y < ey; y++)
    for(int x = sx; x < ex; x++)
        ReadVoxelData(volume, x, y, z, &voxels[Get3DArrayIndex(x,y,z,w,h)]);

    return voxels;
}

static VoxelMeshList GetVoxelMeshList( MeshChunkGenerator* generator,
                                       const Voxel* voxel )
{
    VoxelMeshList list;
    VoxelMesh** meshes = NULL;
    list.meshCount = GatherPayloadFromBitField(generator->meshConditions,
                                               voxel,
                                               sizeof(Voxel),
                                               (void***)&list.meshes);
    return list;
}

static void FreeVoxelMeshListContents( const VoxelMeshList* list )
{
    FreePayloadList((void**)list->meshes);
}

static VoxelMeshList* GatherVoxelMeshListsForChunk( MeshChunkGenerator* generator,
                                                    const Voxel* voxels,
                                                    int w, int h, int d )
{
    const int voxelCount = w*h*d;
    VoxelMeshList* meshLists = new VoxelMeshList[voxelCount];

    for(int z = 0; z < d; z++)
    for(int y = 0; y < h; y++)
    for(int x = 0; x < w; x++)
    {
        const int index = Get3DArrayIndex(x,y,z,w,h);
        const Voxel* voxel = &voxels[index];
        meshLists[index] = GetVoxelMeshList(generator, voxel);
    }

    return meshLists;
}

static bool* GatherTransparentVoxelsForChunk( const VoxelMeshList* meshLists,
                                              int w, int h, int d )
{
    const int voxelCount = w*h*d;
    bool* transparentVoxels = new bool[voxelCount];
    for(int i = 0; i < voxelCount; i++)
    {
        const VoxelMesh* meshes = meshLists[i].meshes;
        const int meshCount     = meshLists[i].meshCount;
        bool transparent = true;
        for(int j = 0; j < meshCount; j++)
        {
            if(!IsVoxelMeshTransparent(&meshes[j]))
            {
                transparent = false;
                break;
            }
        }
        transparentVoxels[i] = transparent;
    }
    return transparentVoxels;
}

static int GetTransparentMooreNeighbourhood( const bool* transparentVoxels,
                                             int x, int y, int z,
                                             int w, int h, int d )
{
    assert(x >= 1 && x <= w-2);
    assert(y >= 1 && y <= h-2);
    assert(z >= 1 && z <= d-2);
    int transparentNeighbours = 0;
#define TEST(xo, yo, zo, flag) \
    if(transparentVoxels[Get3DArrayIndex(x+(xo),y+(yo),z+(zo),w,h)]) \
        transparentNeighbours |= (flag);
    TEST(+1,0,0,MOORE_POSITIVE_X)
    TEST(-1,0,0,MOORE_NEGATIVE_X)
    TEST(0,+1,0,MOORE_POSITIVE_Y)
    TEST(0,-1,0,MOORE_NEGATIVE_Y)
    TEST(0,0,+1,MOORE_POSITIVE_Z)
    TEST(0,0,-1,MOORE_NEGATIVE_Z)
#undef TEST
    return transparentNeighbours;
}

static int* GatherTransparentNeighboursForChunk( const bool* transparentVoxels,
                                                 int w, int h, int d )
{
    const int voxelCount = w*h*d;
    int* transparentNeighbours = new int[voxelCount];
    memset(transparentNeighbours, 0, sizeof(int)*voxelCount);
    for(int z = 1; z < d-1; z++)
    for(int y = 1; y < h-1; y++)
    for(int x = 1; x < w-1; x++)
        transparentNeighbours[Get3DArrayIndex(x,y,z,w,h)] =
            GetTransparentMooreNeighbourhood(transparentVoxels,
                                             x, y, z,
                                             w, h, d);
    return transparentNeighbours;
}

static ChunkEnvironment* CreateChunkEnvironment( MeshChunkGenerator* generator,
                                                 VoxelVolume* volume,
                                                 int sx, int sy, int sz,
                                                 int w, int h, int d )
{
    ChunkEnvironment* env = new ChunkEnvironment;
    memset(env, 0, sizeof(ChunkEnvironment));
    env->w = w;
    env->h = h;
    env->d = d;
    env->voxels = ReadVoxelChunk(volume,
                                 sx, sy, sz,
                                  w,  h,  d);
    env->meshLists = GatherVoxelMeshListsForChunk(generator,
                                                  env->voxels,
                                                  w, h, d);
    env->transparentVoxels = GatherTransparentVoxelsForChunk(env->meshLists,
                                                             w, h, d);
    env->transparentNeighbours =
        GatherTransparentNeighboursForChunk(env->transparentVoxels,
                                            w, h, d);
    return env;
}

static void FreeChunkEnvironment( ChunkEnvironment* env )
{
    const int voxelCount = env->w *
                           env->h *
                           env->d;
    for(int i = 0; i < voxelCount; i++)
        FreeVoxelMeshListContents(&env->meshLists[i]);

    delete[] env->voxels;
    delete[] env->meshLists;
    delete[] env->transparentVoxels;
    delete[] env->transparentNeighbours;

    for(int i = 0; i < env->materialCount; i++)
        ReleaseMeshBuffer(env->materialMeshBuffers[i]);
    free(env->materialMeshBuffers);
    free(env->materialIds);

    delete env;
}

static MeshBuffer* GetMeshBufferForMaterial( ChunkEnvironment* env,
                                             int materialId )
{
    int materialCount = env->materialCount;

    // Look if the material has already been used:
    for(int i = 0; i < materialCount; i++)
        if(env->materialIds[i] == materialId)
            return env->materialMeshBuffers[i];

    // Create a new entry:

    // Extend the arrays by one:
    materialCount++;
    env->materialCount = materialCount;
    env->materialMeshBuffers =
        (MeshBuffer**)realloc(env->materialMeshBuffers,
                              sizeof(MeshBuffer*)*materialCount);
    env->materialIds =
        (int*)realloc(env->materialIds,
                      sizeof(int)*materialCount);

    // Insert the material:
    MeshBuffer* buffer = CreateMeshBuffer();
    env->materialIds[materialCount-1] = materialId;
    env->materialMeshBuffers[materialCount-1] = buffer;

    return buffer;
}

// ----------------------------------------------------------------------

static void ProcessBlockVoxelMesh( ChunkEnvironment* env,
                                   int transparentNeighbours,
                                   MeshBuffer* materialMeshBuffer,
                                   int x, int y, int z,
                                   const BlockVoxelMesh* mesh )
{
    const MeshBuffer* const* meshBuffers = &mesh->meshBuffers[0];

    const glm::mat4 transform = translate(glm::mat4(1.f),
                                          glm::vec3(x, y, z));

    if(transparentNeighbours != 0 &&
       meshBuffers[CENTER])
    {
        AppendMeshBuffer(materialMeshBuffer,
                         meshBuffers[CENTER],
                         &transform);
    }

#define GEN_DIR(dir) \
    if(transparentNeighbours & MOORE_##dir && meshBuffers[dir]) \
    { \
        AppendMeshBuffer(materialMeshBuffer, meshBuffers[dir], &transform); \
    }
    GEN_DIR(POSITIVE_X)
    GEN_DIR(NEGATIVE_X)
    GEN_DIR(POSITIVE_Y)
    GEN_DIR(NEGATIVE_Y)
    GEN_DIR(POSITIVE_Z)
    GEN_DIR(NEGATIVE_Z)
#undef GEN_DIR
}

static void ProcessVoxelMesh( ChunkEnvironment* env,
                              int transparentNeighbours,
                              int x, int y, int z,
                              const VoxelMesh* mesh )
{
    MeshBuffer* materialMeshBuffer =
        GetMeshBufferForMaterial(env, mesh->materialId);
    switch(mesh->type)
    {
        case BLOCK_VOXEL_MESH:
            ProcessBlockVoxelMesh(env,
                                  transparentNeighbours,
                                  materialMeshBuffer,
                                  x, y, z,
                                  (BlockVoxelMesh*)&mesh->data);
            return;
    }
    FatalError("Unknown voxel mesh type.");
}

static MeshChunk* GenerateMeshChunkWithEnv( ChunkEnvironment* env )
{
    MeshChunk* chunk = new MeshChunk;
    memset(chunk, 0, sizeof(MeshChunk));

    const int w = env->w-2;
    const int h = env->h-2;
    const int d = env->d-2;
    const int voxelCount = w*h*d;

    for(int z = 0; z < d; z++)
    for(int y = 0; y < h; y++)
    for(int x = 0; x < w; x++)
    {
        const int envIndex = Get3DArrayIndex(x+1,
                                             y+1,
                                             z+1,
                                             w-2,
                                             h-2);
        int transparentNeighbours    = env->transparentNeighbours[envIndex];
        const VoxelMeshList meshList = env->meshLists[envIndex];

        for(int i = 0; i < meshList.meshCount; i++)
        {
            VoxelMesh* mesh = &meshList.meshes[i];
            ProcessVoxelMesh(env,
                             transparentNeighbours,
                             x, y, z,
                             mesh);
        }
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
    return NULL;
}

void FreeMeshChunk( MeshChunk* chunk )
{
    for(int i = 0; i < chunk->materialCount; i++)
        ReleaseMesh(chunk->materialMeshes[i]);
    delete[] chunk->materialMeshes;
    delete[] chunk->materialIds;
    delete chunk;
}
