#include <string.h> // strcmp

#include "../Lua.h"
#include "../ChunkGenerator.h"
#include "MeshBuffer.h"
#include "PhysicsManager.h"
#include "ChunkGenerator.h"


// ---- ChunkGenerator ----

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
        lua_createtable(l, 0, chunk->meshCount);
        for(int i = 0; i < chunk->meshCount; i++)
        {
            PushPointerToLua(l, chunk->meshes[i]);
            lua_rawseti(l, -2, i+1);
        }

        lua_createtable(l, 0, chunk->meshCount);
        for(int i = 0; i < chunk->meshCount; i++)
        {
            lua_pushinteger(l, chunk->meshIds[i]);
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


// ---- VoxelRepresentation ----

static int Lua_CreateVoxelRepresentation( lua_State* l )
{
    ChunkGenerator* generator = CheckChunkGeneratorFromLua(l, 1);
    VoxelRepresentation* representation = CreateVoxelRepresentation(generator);
    if(representation)
    {
        PushPointerToLua(l, representation);
        return 1;
    }
    else
    {
        luaL_error(l, "Failed to create voxel representation!");
        return 0;
    }
}

static int Lua_SetVoxelRepresentationOpeningState( lua_State* l )
{
    VoxelRepresentation* representation = CheckVoxelRepresentationFromLua(l, 1);

    static const char* stateNames[] =
    {
        "closed",
        "transparent",
        "open",
        NULL
    };
    const VoxelRepresentationOpeningState state =
        (VoxelRepresentationOpeningState)luaL_checkoption(l, 2, NULL, stateNames);

    SetVoxelRepresentationOpeningState(representation, state);
}

static int Lua_AddMeshToVoxelRepresentation( lua_State* l )
{
    VoxelRepresentation* representation = CheckVoxelRepresentationFromLua(l, 1);
    const int meshId = luaL_checkinteger(l, 2);
    const int meshBufferCount = lua_gettop(l)-2;
    MeshBuffer** meshBuffers = new MeshBuffer*[meshBufferCount];
    for(int i = 0; i < meshBufferCount; i++)
    {
        MeshBuffer* meshBuffer = CheckMeshBufferFromLua(l, 3+i);
        meshBuffers[i] = meshBuffer;
    }
    AddMeshToVoxelRepresentation(representation, meshId, meshBuffers);
    delete[] meshBuffers;
    return 1;
}

static int Lua_AddCollisionShapeToVoxelRepresentation( lua_State* l )
{
    VoxelRepresentation* representation = CheckVoxelRepresentationFromLua(l, 1);
    CollisionShape* collisionShape = CheckCollisionShapeFromLua(l, 2);
    AddCollisionShapeToVoxelRepresentation(representation, collisionShape);
    return 1;
}


ChunkGenerator* GetChunkGeneratorFromLua( lua_State* l, int stackPosition )
{
    return (ChunkGenerator*)GetPointerFromLua(l, stackPosition);
}

ChunkGenerator* CheckChunkGeneratorFromLua( lua_State* l, int stackPosition )
{
    return (ChunkGenerator*)CheckPointerFromLua(l, stackPosition);
}

VoxelRepresentation* GetVoxelRepresentationFromLua( lua_State* l, int stackPosition )
{
    return (VoxelRepresentation*)GetPointerFromLua(l, stackPosition);
}

VoxelRepresentation* CheckVoxelRepresentationFromLua( lua_State* l, int stackPosition )
{
    return (VoxelRepresentation*)CheckPointerFromLua(l, stackPosition);
}

bool RegisterChunkGeneratorInLua()
{
    return
        RegisterFunctionInLua("CreateChunkGenerator", Lua_CreateChunkGenerator) &&
        RegisterFunctionInLua("DestroyChunkGenerator", Lua_DestroyChunkGenerator) &&
        RegisterFunctionInLua("GenerateChunk", Lua_GenerateChunk) &&
        RegisterFunctionInLua("CreateVoxelRepresentation", Lua_CreateVoxelRepresentation) &&
        RegisterFunctionInLua("SetVoxelRepresentationOpeningState", Lua_SetVoxelRepresentationOpeningState) &&
        RegisterFunctionInLua("AddMeshToVoxelRepresentation", Lua_AddMeshToVoxelRepresentation) &&
        RegisterFunctionInLua("AddCollisionShapeToVoxelRepresentation", Lua_AddCollisionShapeToVoxelRepresentation);
}
