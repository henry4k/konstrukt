#include <string.h> // memset
#include "Common.h"
#include "Reference.h"
#include "VoxelVolume.h"


struct VoxelVolume
{
    ReferenceCounter refCounter;
    int size[3];
    Voxel* voxels;
};


VoxelVolume* CreateVoxelVolume( int width, int height, int depth )
{
    VoxelVolume* volume = new VoxelVolume;
    memset(volume, 0, sizeof(VoxelVolume));
    InitReferenceCounter(&volume->refCounter);
    volume->size[0] = width;
    volume->size[1] = height;
    volume->size[2] = depth;
    volume->voxels = new Voxel[width*height*depth];
    memset(volume->voxels, 0, sizeof(Voxel)*width*height*depth);
    return volume;
}

static void FreeVoxelVolume( VoxelVolume* volume )
{
    delete[] volume->voxels;
    delete volume;
}

void ReferenceVoxelVolume( VoxelVolume* volume )
{
    Reference(&volume->refCounter);
}

void ReleaseVoxelVolume( VoxelVolume* volume )
{
    Release(&volume->refCounter);
    if(!HasReferences(&volume->refCounter))
        FreeVoxelVolume(volume);
}

static int GetVoxelIndex( VoxelVolume* volume, int x, int y, int z )
{
    if(x < 0 || y < 0 || z < 0 ||
       x >= volume->size[0] ||
       y >= volume->size[1] ||
       z >= volume->size[2])
        return -1;

    return z * volume->size[0] * volume->size[1] +
           y * volume->size[0] +
           x;
}

bool ReadVoxelData( VoxelVolume* volume, int x, int y, int z, Voxel* destination )
{
    const int index = GetVoxelIndex(volume, x,y,z);
    if(index < 0)
        return false;

    const Voxel* voxel = &volume->voxels[index];
    memcpy(destination, voxel, sizeof(Voxel));
    return true;
}

bool WriteVoxelData( VoxelVolume* volume, int x, int y, int z, const Voxel* source )
{
    const int index = GetVoxelIndex(volume, x,y,z);
    if(index < 0)
        return false;

    Voxel* voxel = &volume->voxels[index];
    memcpy(voxel, source, sizeof(Voxel));
    return true;
}
