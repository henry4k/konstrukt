#include <stdint.h>
#include "../Lua.h"
#include "../VoxelVolume.h"
#include "VoxelVolume.h"


static const int VOXEL_INT32_COUNT = VOXEL_SIZE/4;


static int Lua_SetVoxelVolumeSize( lua_State* l )
{
    const int width  = luaL_checkinteger(l, 1);
    const int height = luaL_checkinteger(l, 2);
    const int depth  = luaL_checkinteger(l, 3);
    SetVoxelVolumeSize(width, height, depth);
    return 0;
}

static int Lua_ReadVoxelData( lua_State* l )
{
    const int x = luaL_checkinteger(l, 1);
    const int y = luaL_checkinteger(l, 2);
    const int z = luaL_checkinteger(l, 3);

    int32_t voxel[VOXEL_INT32_COUNT];
    const bool success = ReadVoxelData(x, y, z, voxel);
    if(!success)
    {
        luaL_error(l, "Can't read voxel data.");
        return 0;
    }

    lua_createtable(l, VOXEL_INT32_COUNT, 0);
    for(int i = 0; i < VOXEL_INT32_COUNT; i++)
    {
        lua_pushinteger(l, voxel[i]);
        lua_rawseti(l, -2, i+1);
    }
    return 1;
}

static int Lua_WriteVoxelData( lua_State* l )
{
    const int x = luaL_checkinteger(l, 1);
    const int y = luaL_checkinteger(l, 2);
    const int z = luaL_checkinteger(l, 3);
    // Voxel data table at index 4

    int32_t voxel[VOXEL_INT32_COUNT];
    for(int i = 0; i < VOXEL_INT32_COUNT; i++)
    {
        lua_rawgeti(l, 4, i+1);
        voxel[i] = lua_tointeger(l, -1);
        lua_pop(l, 1);
    }

    const bool success = WriteVoxelData(x, y, z, voxel);
    if(!success)
    {
        luaL_error(l, "Can't write voxel data.");
        return 0;
    }

    return 0;
}

bool RegisterVoxelVolumeInLua()
{
    return
        RegisterFunctionInLua("SetVoxelVolumeSize", Lua_SetVoxelVolumeSize) &&
        RegisterFunctionInLua("ReadVoxelData", Lua_ReadVoxelData) &&
        RegisterFunctionInLua("WriteVoxelData", Lua_WriteVoxelData);
}
