#ifndef __APOAPSIS_BIT_CONDITION__
#define __APOAPSIS_BIT_CONDITION__

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
void FreePayloadList( void** payloadList );

#endif
