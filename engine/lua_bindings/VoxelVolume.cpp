#include <stdint.h>
#include "../Lua.h"
#include "../VoxelVolume.h"
#include "VoxelVolume.h"


static const int VOXEL_INT32_COUNT = sizeof(Voxel)/4;


static int Lua_CreateVoxelVolume( lua_State* l )
{
    const int width  = luaL_checkinteger(l, 1);
    const int height = luaL_checkinteger(l, 2);
    const int depth  = luaL_checkinteger(l, 3);
    VoxelVolume* volume = CreateVoxelVolume(width, height, depth);
    if(volume)
    {
        PushPointerToLua(l, volume);
        ReferenceVoxelVolume(volume);
        return 1;
    }
    else
    {
        luaL_error(l, "Failed to create voxel volume!");
        return 0;
    }
}

static int Lua_DestroyVoxelVolume( lua_State* l )
{
    VoxelVolume* volume = CheckVoxelVolumeFromLua(l, 1);
    ReleaseVoxelVolume(volume);
    return 0;
}

static int Lua_ReadVoxelData( lua_State* l )
{
    VoxelVolume* volume = CheckVoxelVolumeFromLua(l, 1);
    const int x = luaL_checkinteger(l, 2);
    const int y = luaL_checkinteger(l, 3);
    const int z = luaL_checkinteger(l, 4);

    Voxel voxel;
    const bool success = ReadVoxelData(volume, x, y, z, &voxel);
    if(!success)
    {
        lua_pushnil(l);
        return 1;
    }

    const int32_t* voxelData = (const int32_t*)&voxel;
    lua_createtable(l, VOXEL_INT32_COUNT, 0);
    for(int i = 0; i < VOXEL_INT32_COUNT; i++)
    {
        lua_pushinteger(l, voxelData[i]);
        lua_rawseti(l, -2, i+1);
    }
    return 1;
}

static int Lua_WriteVoxelData( lua_State* l )
{
    VoxelVolume* volume = CheckVoxelVolumeFromLua(l, 1);
    const int x = luaL_checkinteger(l, 2);
    const int y = luaL_checkinteger(l, 3);
    const int z = luaL_checkinteger(l, 4);
    // Voxel data table at index 5

    int32_t voxelData[VOXEL_INT32_COUNT];
    for(int i = 0; i < VOXEL_INT32_COUNT; i++)
    {
        lua_rawgeti(l, 5, i+1);
        voxelData[i] = lua_tointeger(l, -1);
        lua_pop(l, 1);
    }

    const Voxel* voxel = (const Voxel*)&voxelData;
    const bool success = WriteVoxelData(volume, x, y, z, voxel);
    if(!success)
    {
        lua_pushboolean(l, false);
        return 1;
    }

    lua_pushboolean(l, true);
    return 1;
}

static int Lua_GetVoxelInt32Count( lua_State* l )
{
    lua_pushinteger(l, VOXEL_INT32_COUNT);
    return 1;
}

VoxelVolume* GetVoxelVolumeFromLua( lua_State* l, int stackPosition )
{
    return (VoxelVolume*)GetPointerFromLua(l, stackPosition);
}

VoxelVolume* CheckVoxelVolumeFromLua( lua_State* l, int stackPosition )
{
    return (VoxelVolume*)CheckPointerFromLua(l, stackPosition);
}

void RegisterVoxelVolumeInLua()
{
    RegisterFunctionInLua("CreateVoxelVolume", Lua_CreateVoxelVolume);
    RegisterFunctionInLua("DestroyVoxelVolume", Lua_DestroyVoxelVolume);
    RegisterFunctionInLua("ReadVoxelData", Lua_ReadVoxelData);
    RegisterFunctionInLua("WriteVoxelData", Lua_WriteVoxelData);
    RegisterFunctionInLua("GetVoxelInt32Count", Lua_GetVoxelInt32Count);
}
