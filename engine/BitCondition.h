#ifndef __KONSTRUKT_BIT_CONDITION__
#define __KONSTRUKT_BIT_CONDITION__

/**
 * Used to extract and test bits from a bit field.
 */
struct BitCondition
{
    int offset; /** Position in the bit field. */
    int length; /** Amount of bits extracted for comparision. */
    int value;  /** Value to which the bits are compared to. */
};

struct BitFieldPayloadList
{
    int length;
    void** data;
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
 * @return
 * A list which contains the results.
 */
BitFieldPayloadList GatherPayloadFromBitField( const BitConditionSolver* solver,
                                               const void* bitField,
                                               int bitFieldSize );

void DestroyBitFieldPayloadList( BitFieldPayloadList* list );

#endif
