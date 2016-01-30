#ifndef __KONSTRUKT_VOXEL_VOLUME__
#define __KONSTRUKT_VOXEL_VOLUME__

struct VoxelVolume;

struct Voxel
{
    char data[16];
};


/**
 * Creates a voxel volume with the given size.
 */
VoxelVolume* CreateVoxelVolume( int width, int height, int depth );

void ReferenceVoxelVolume( VoxelVolume* volume );
void ReleaseVoxelVolume( VoxelVolume* volume );

/**
 * @param destination
 * Writes #VOXEL_SIZE bytes to `destination`.
 *
 * @return whether the read operation was successfull.
 */
bool ReadVoxelData( VoxelVolume* volume, int x, int y, int z, Voxel* destination );

/**
 * @param source
 * Reads #VOXEL_SIZE bytes from `source`.
 *
 * @return whether the write operation was successfull.
 */
bool WriteVoxelData( VoxelVolume* volume, int x, int y, int z, const Voxel* source );

#endif
