#ifndef __APOAPSIS_LUA_BINDINGS_VOXEL_VOLUME__
#define __APOAPSIS_LUA_BINDINGS_VOXEL_VOLUME__

struct lua_State;
struct VoxelVolume;

VoxelVolume* GetVoxelVolumeFromLua( lua_State* l, int stackPosition );
VoxelVolume* CheckVoxelVolumeFromLua( lua_State* l, int stackPosition );

bool RegisterVoxelVolumeInLua();

#endif
