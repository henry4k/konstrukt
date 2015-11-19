#ifndef __APOAPSIS_CHUNK_GENERATOR_SHARED__
#define __APOAPSIS_CHUNK_GENERATOR_SHARED__

enum VoxelOpeningState
{
    VOXEL_CLOSED,
    VOXEL_TRANSPARENT,
    VOXEL_OPEN
};


struct BitCondition
{
    int offset;
    int length;
    int value;
};

struct BitConditionSolver;


BitConditionSolver* CreateBitConditionSolver();
void FreeBitConditionSolver( BitConditionSolver* solver );
void AddBitConditions( BitConditionSolver* solver,
                       const BitCondition* conditions,
                       int conditionCount,
                       void* payload );
int GatherPayloadFromBitField( const BitConditionSolver* solver,
                               const void* bitField,
                               int bitFieldSize,
                               void*** payloadList );

#endif
