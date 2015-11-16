#include <string.h> // strcmp

#include "../Lua.h"
#include "../ChunkGenerator.h"
#include "MeshBuffer.h"
#include "PhysicsManager.h"
#include "ChunkGenerator.h"


static int Lua_CreateChunkGenerator( lua_State* l )
{
    ChunkGenerator* generator = CreateChunkGenerator();
    if(generator)
    {
        PushPointerToLua(l, generator);
        ReferenceChunkGenerator(generator);
        return 1;
    }
    else
    {
        luaL_error(l, "Failed to create chunk generator!");
        return 0;
    }
}

static int Lua_DestroyChunkGenerator( lua_State* l )
{
    ChunkGenerator* generator = CheckChunkGeneratorFromLua(l, 1);
    ReleaseChunkGenerator(generator);
    return 0;
}

static void GetBlockVoxelRepresentationMaterialFromLua( lua_State* l,
                                                    BlockVoxelRepresentationMaterial* mesh )
{
    lua_rawgeti(l, -1, 1);
    mesh->id = luaL_checkinteger(l, -1);
    lua_pop(l, 1);
    for(int i = 0; i < BLOCK_VOXEL_REPRESENTATION_MATERIAL_BUFFER_COUNT; i++)
    {
        lua_rawgeti(l, -1, i+2);
        mesh->buffers[i] = CheckMeshBufferFromLua(l, -1);
        lua_pop(l, 1);
    }
}

static int Lua_CreateBlockVoxelRepresentation( lua_State* l )
{
    ChunkGenerator* generator = CheckChunkGeneratorFromLua(l, 1);

    static const char* stateNames[] =
    {
        "closed",
        "transparent",
        "open",
        NULL
    };
    const VoxelRepresentationOpeningState state =
        (VoxelRepresentationOpeningState)luaL_checkoption(l, 2, NULL, stateNames);

    const int meshCount = lua_rawlen(l, 3);
    BlockVoxelRepresentationMaterial* meshes =
        new BlockVoxelRepresentationMaterial[meshCount];
    for(int i = 0; i < meshCount; i++)
    {
        lua_rawgeti(l, 3, i+1);
        GetBlockVoxelRepresentationMaterialFromLua(l, &meshes[i]);
        lua_pop(l, 1);
    }

    const int collisionShapeCount = lua_rawlen(l, 4);
    CollisionShape** collisionShapes =
        new CollisionShape*[collisionShapeCount];
    for(int i = 0; i < collisionShapeCount; i++)
    {
        lua_rawgeti(l, 4, i+1);
        collisionShapes[i] = CheckCollisionShapeFromLua(l, -1);
        lua_pop(l, 1);
    }

    bool result = CreateBlockVoxelRepresentation(generator,
                                                 state,
                                                 meshes,
                                                 meshCount,
                                                 collisionShapes,
                                                 collisionShapeCount);

    delete[] meshes;
    delete[] collisionShapes;

    if(result == false)
        luaL_error(l, "Can't create block voxel representation!");
    return 0;
}

static int Lua_GenerateChunk( lua_State* l )
{
    ChunkGenerator* generator = CheckChunkGeneratorFromLua(l, 1);
    const int x = luaL_checkinteger(l, 2);
    const int y = luaL_checkinteger(l, 3);
    const int z = luaL_checkinteger(l, 4);
    const int w = luaL_checkinteger(l, 5);
    const int h = luaL_checkinteger(l, 6);
    const int d = luaL_checkinteger(l, 7);

    Chunk* chunk = GenerateChunk(generator,
                                 x, y, z,
                                 w, h, d);
    if(chunk)
    {
        lua_createtable(l, 0, chunk->materialCount);
        for(int i = 0; i < chunk->materialCount; i++)
        {
            PushPointerToLua(l, chunk->materialMeshes[i]);
            lua_rawseti(l, -2, i+1);
        }

        lua_createtable(l, 0, chunk->materialCount);
        for(int i = 0; i < chunk->materialCount; i++)
        {
            lua_pushinteger(l, chunk->materialIds[i]);
            lua_rawseti(l, -2, i+1);
        }

        lua_createtable(l, 0, chunk->collisionShapeCount);
        for(int i = 0; i < chunk->collisionShapeCount; i++)
        {
            PushPointerToLua(l, chunk->collisionShapes[i]);
            lua_rawseti(l, -2, i+1);
        }

        FreeChunk(chunk);
        return 3;
    }
    else
    {
        luaL_error(l, "Failed to generate chunk!");
        return 0;
    }
}


ChunkGenerator* GetChunkGeneratorFromLua( lua_State* l, int stackPosition )
{
    return (ChunkGenerator*)GetPointerFromLua(l, stackPosition);
}

ChunkGenerator* CheckChunkGeneratorFromLua( lua_State* l, int stackPosition )
{
    return (ChunkGenerator*)CheckPointerFromLua(l, stackPosition);
}

bool RegisterChunkGeneratorInLua()
{
    return
        RegisterFunctionInLua("CreateChunkGenerator", Lua_CreateChunkGenerator) &&
        RegisterFunctionInLua("DestroyChunkGenerator", Lua_DestroyChunkGenerator) &&
        RegisterFunctionInLua("CreateBlockVoxelRepresentation", Lua_CreateBlockVoxelRepresentation) &&
        RegisterFunctionInLua("GenerateChunk", Lua_GenerateChunk);
}
