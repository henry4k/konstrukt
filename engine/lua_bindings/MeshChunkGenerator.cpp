#include <assert.h>
#include <string.h> // memset

#include "../Lua.h"
#include "../MeshChunkGenerator.h"
#include "MeshBuffer.h"
#include "PhysicsManager.h"
#include "VoxelVolume.h"
#include "MeshChunkGenerator.h"


static int Lua_CreateMeshChunkGenerator( lua_State* l )
{
    MeshChunkGenerator* generator = CreateMeshChunkGenerator();
    if(generator)
    {
        PushPointerToLua(l, generator);
        ReferenceMeshChunkGenerator(generator);
        return 1;
    }
    else
    {
        luaL_error(l, "Failed to create chunk generator!");
        return 0;
    }
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
    BitCondition* conditions = new BitCondition[conditionCount];
    for(int i = 0; i < conditionCount; i++)
    {
        lua_rawgeti(l, 3, i+1);
        GetBitConditionFromLua(l, &conditions[i]);
        lua_pop(l, 1);
    }

    const bool transparent = (bool)lua_toboolean(l, 4);

    MeshBuffer* meshBuffers[BLOCK_VOXEL_MATERIAL_BUFFER_COUNT];
    memset(meshBuffers, 0, sizeof(MeshBuffer*));
    for(int i = 0; i < BLOCK_VOXEL_MATERIAL_BUFFER_COUNT; i++)
    {
        lua_rawgeti(l, 5, i+1);
        meshBuffers[i] = GetMeshBufferFromLua(l, -1);
        lua_pop(l, 1);
    }

    bool result = CreateBlockVoxelMesh(generator,
                                       materialId,
                                       conditions,
                                       conditionCount,
                                       transparent,
                                       meshBuffers);

    delete[] conditions;

    if(result == false)
        luaL_error(l, "Can't create block voxel representation!");
    return 0;
}

static int Lua_GenerateMeshChunk( lua_State* l )
{
    MeshChunkGenerator* generator = CheckMeshChunkGeneratorFromLua(l, 1);
    VoxelVolume* volume           = CheckVoxelVolumeFromLua(l, 2);
    const int x = luaL_checkinteger(l, 3);
    const int y = luaL_checkinteger(l, 4);
    const int z = luaL_checkinteger(l, 5);
    const int w = luaL_checkinteger(l, 6);
    const int h = luaL_checkinteger(l, 7);
    const int d = luaL_checkinteger(l, 8);

    MeshChunk* chunk = GenerateMeshChunk(generator,
                                         volume,
                                         x, y, z,
                                         w, h, d);
    if(chunk)
    {
        lua_createtable(l, chunk->materialCount, 0);
        for(int i = 0; i < chunk->materialCount; i++)
        {
            PushPointerToLua(l, chunk->materialMeshes[i]);
            lua_rawseti(l, -2, i+1);
        }

        lua_createtable(l, chunk->materialCount, 0);
        for(int i = 0; i < chunk->materialCount; i++)
        {
            lua_pushinteger(l, chunk->materialIds[i]);
            lua_rawseti(l, -2, i+1);
        }

        FreeMeshChunk(chunk);
        return 2;
    }
    else
    {
        luaL_error(l, "Failed to generate chunk!");
        return 0;
    }
}


MeshChunkGenerator* GetMeshChunkGeneratorFromLua( lua_State* l, int stackPosition )
{
    return (MeshChunkGenerator*)GetPointerFromLua(l, stackPosition);
}

MeshChunkGenerator* CheckMeshChunkGeneratorFromLua( lua_State* l, int stackPosition )
{
    return (MeshChunkGenerator*)CheckPointerFromLua(l, stackPosition);
}

bool RegisterMeshChunkGeneratorInLua()
{
    return
        RegisterFunctionInLua("CreateMeshChunkGenerator", Lua_CreateMeshChunkGenerator) &&
        RegisterFunctionInLua("DestroyMeshChunkGenerator", Lua_DestroyMeshChunkGenerator) &&
        RegisterFunctionInLua("CreateBlockVoxelMesh", Lua_CreateBlockVoxelMesh) &&
        RegisterFunctionInLua("GenerateMeshChunk", Lua_GenerateMeshChunk);
}
