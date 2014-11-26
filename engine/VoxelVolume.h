#ifndef __APOAPSIS_VOXEL_VOLUME__
#define __APOAPSIS_VOXEL_VOLUME__

static const int VOXEL_SIZE = 16;


bool InitVoxelVolume();
void DestroyVoxelVolume();

/**
 * Recreates the voxel volume with the given size.
 */
void SetVoxelVolumeSize( int width, int height, int depth );

/**
 * @param destination
 * Reads #VOXEL_SIZE bytes to `destination`.
 *
 * @return whether the read operation was successfull.
 */
bool ReadVoxelData( int x, int y, int z, void* destination );

/**
 * @param source
 * Reads #VOXEL_SIZE bytes from `source`.
 *
 * @return whether the write operation was successfull.
 */
bool WriteVoxelData( int x, int y, int z, const void* source );

#endif
