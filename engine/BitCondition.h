#ifndef __APOAPSIS_BIT_CONDITION__
#define __APOAPSIS_BIT_CONDITION__

struct List;


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
 * @return
 * A list which contains the results or `NULL` if something went wrong.
 */
List* GatherPayloadFromBitField( const BitConditionSolver* solver,
                                 const void* bitField,
                                 int bitFieldSize );

#endif
