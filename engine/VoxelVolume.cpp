#include <string.h> // memset, memcpy
#include "Common.h"
#include "VoxelVolume.h"


struct Voxel
{
    // CA data goes here!
    char userData[VOXEL_SIZE];
};


static int VoxelVolumeSize[3] = {0, 0, 0};
static Voxel* VoxelVolume = NULL;


bool InitVoxelVolume()
{
    return true;
}

void DestroyVoxelVolume()
{
    if(VoxelVolume)
        delete[] VoxelVolume;
}

void SetVoxelVolumeSize( int width, int height, int depth )
{
    VoxelVolumeSize[0] = width;
    VoxelVolumeSize[1] = height;
    VoxelVolumeSize[2] = depth;

    if(VoxelVolume)
        delete[] VoxelVolume;

    const int voxelCount = width * height * depth;
    VoxelVolume = new Voxel[voxelCount];
    memset(VoxelVolume, 0, sizeof(Voxel)*voxelCount);
}

static int GetVoxelIndex( int x, int y, int z )
{
    if(x < 0 || y < 0 || z < 0 ||
       x >= VoxelVolumeSize[0] ||
       y >= VoxelVolumeSize[1] ||
       z >= VoxelVolumeSize[2])
        return -1;

    return z * VoxelVolumeSize[0] * VoxelVolumeSize[1] +
           y * VoxelVolumeSize[0] +
           x;
}

bool ReadVoxelData( int x, int y, int z, void* destination )
{
    const int index = GetVoxelIndex(x,y,z);
    if(index < 0)
        return false;

    const Voxel* voxel = &VoxelVolume[index];
    memcpy(destination, voxel->userData, VOXEL_SIZE);
    return true;
}

bool WriteVoxelData( int x, int y, int z, const void* source )
{
    const int index = GetVoxelIndex(x,y,z);
    if(index < 0)
        return false;

    Voxel* voxel = &VoxelVolume[index];
    memcpy(voxel->userData, source, VOXEL_SIZE);
    return true;
}
