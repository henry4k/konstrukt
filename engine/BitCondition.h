#ifndef __APOAPSIS_BIT_CONDITION__
#define __APOAPSIS_BIT_CONDITION__

/**
 * Used to extract and test bits from a bit field.
 */
struct BitCondition
{
    int offset; /** Position in the bit field. */
    int length; /** Amount of bits extracted for comparision. */
    int value;  /** Value to which the bits are compared to. */
};

/**
 * Stores arbitrary values (payloads), which can be found, when a bit field
 * matches a set of bit conditions.
 */
struct BitConditionSolver;


BitConditionSolver* CreateBitConditionSolver();

void FreeBitConditionSolver( BitConditionSolver* solver );

/**
 * Store a value (payload) for the given set of bit conditions.
 */
void AddBitConditions( BitConditionSolver* solver,
                       const BitCondition* conditions,
                       int conditionCount,
                       void* payload );

/**
 * Collect all values (payloads), whose bit conditions match the given bit field.
 *
 * @param payloadList
 * The result list is written to this pointer.
 * Writes `NULL` if there are no results.
 * Use #FreePayloadList to release the list, when you don't need it anymore.
 *
 * @return
 * The result count.
 */
int GatherPayloadFromBitField( const BitConditionSolver* solver,
                               const void* bitField,
                               int bitFieldSize,
                               void*** payloadList );

void FreePayloadList( void** payloadList );

#endif
