#include <assert.h>
#include <string.h> // memset

#include "../Common.h"
#include "../Lua.h"
#include "../Mesh.h"
#include "../MeshChunkGenerator.h"
#include "Math.h"
#include "MeshBuffer.h"
#include "VoxelVolume.h"
#include "JobManager.h"
#include "MeshChunkGenerator.h"


static int Lua_CreateMeshChunkGenerator( lua_State* l )
{
    MeshChunkGenerator* generator = CreateMeshChunkGenerator();
    PushPointerToLua(l, generator);
    ReferenceMeshChunkGenerator(generator);
    return 1;
}

static int Lua_DestroyMeshChunkGenerator( lua_State* l )
{
    MeshChunkGenerator* generator = CheckMeshChunkGeneratorFromLua(l, 1);
    ReleaseMeshChunkGenerator(generator);
    return 0;
}

static void GetBitConditionFromLua( lua_State* l, BitCondition* condition )
{
    lua_rawgeti(l, -1, 1);
    condition->offset = luaL_checkinteger(l, -1);
    lua_rawgeti(l, -2, 2);
    condition->length = luaL_checkinteger(l, -1);
    lua_rawgeti(l, -3, 3);
    condition->value  = luaL_checkinteger(l, -1);
    lua_pop(l, 3);
}

static int Lua_CreateBlockVoxelMesh( lua_State* l )
{
    MeshChunkGenerator* generator = CheckMeshChunkGeneratorFromLua(l, 1);

    const int materialId = luaL_checkinteger(l, 2);

    const int conditionCount = lua_rawlen(l, 3);
    BitCondition* conditions = (BitCondition*)Alloc(sizeof(BitCondition)*conditionCount);
    REPEAT(conditionCount, i)
    {
        lua_rawgeti(l, 3, i+1);
        GetBitConditionFromLua(l, &conditions[i]);
        lua_pop(l, 1);
    }

    const bool transparent = (bool)lua_toboolean(l, 4);

    MeshBuffer* meshBuffers[BLOCK_VOXEL_MATERIAL_BUFFER_COUNT];
    memset(meshBuffers, 0, sizeof(MeshBuffer*)*BLOCK_VOXEL_MATERIAL_BUFFER_COUNT);
    REPEAT(BLOCK_VOXEL_MATERIAL_BUFFER_COUNT, i)
    {
        lua_rawgeti(l, 5, i+1);
        meshBuffers[i] = GetMeshBufferFromLua(l, -1);
        lua_pop(l, 1);
    }

    Mat4 transformations[BLOCK_VOXEL_MATERIAL_BUFFER_COUNT];
    REPEAT(BLOCK_VOXEL_MATERIAL_BUFFER_COUNT, i)
    {
        lua_rawgeti(l, 6, i+1);
        const Mat4* transformation = GetMatrix4FromLua(l, -1);
        if(transformation)
            transformations[i] = *transformation;
        lua_pop(l, 1);
    }

    CreateBlockVoxelMesh(generator,
                         materialId,
                         conditions,
                         conditionCount,
                         transparent,
                         meshBuffers,
                         transformations);

    Free(conditions);
    return 0;
}

static int Lua_BeginGeneratingMeshChunk( lua_State* l )
{
    MeshChunkGenerator* generator = CheckMeshChunkGeneratorFromLua(l, 1);
    VoxelVolume* volume           = CheckVoxelVolumeFromLua(l, 2);
    const int x = luaL_checkinteger(l, 3);
    const int y = luaL_checkinteger(l, 4);
    const int z = luaL_checkinteger(l, 5);
    const int w = luaL_checkinteger(l, 6);
    const int h = luaL_checkinteger(l, 7);
    const int d = luaL_checkinteger(l, 8);

    PushJobToLua(l, BeginGeneratingMeshChunk(generator,
                                             volume,
                                             x, y, z,
                                             w, h, d));
    return 1;
}

static int Lua_GetGeneratedMeshChunk( lua_State* l )
{
    const JobId job = CheckJobFromLua(l, 1);
    MeshChunk* chunk = GetGeneratedMeshChunk(job);

    lua_createtable(l, chunk->materialCount, 0);
    REPEAT(chunk->materialCount, i)
    {
        Mesh* mesh = chunk->materialMeshes[i];
        PushPointerToLua(l, mesh);
        ReferenceMesh(mesh);
        lua_rawseti(l, -2, i+1);
    }

    lua_createtable(l, chunk->materialCount, 0);
    REPEAT(chunk->materialCount, i)
    {
        lua_pushinteger(l, chunk->materialIds[i]);
        lua_rawseti(l, -2, i+1);
    }

    FreeMeshChunk(chunk);
    return 2;
}


MeshChunkGenerator* GetMeshChunkGeneratorFromLua( lua_State* l, int stackPosition )
{
    return (MeshChunkGenerator*)GetPointerFromLua(l, stackPosition);
}

MeshChunkGenerator* CheckMeshChunkGeneratorFromLua( lua_State* l, int stackPosition )
{
    return (MeshChunkGenerator*)CheckPointerFromLua(l, stackPosition);
}

void RegisterMeshChunkGeneratorInLua()
{
    RegisterFunctionInLua("CreateMeshChunkGenerator", Lua_CreateMeshChunkGenerator);
    RegisterFunctionInLua("DestroyMeshChunkGenerator", Lua_DestroyMeshChunkGenerator);
    RegisterFunctionInLua("CreateBlockVoxelMesh", Lua_CreateBlockVoxelMesh);
    RegisterFunctionInLua("BeginGeneratingMeshChunk", Lua_BeginGeneratingMeshChunk);
    RegisterFunctionInLua("GetGeneratedMeshChunk", Lua_GetGeneratedMeshChunk);
}
