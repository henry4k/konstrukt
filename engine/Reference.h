#ifndef __KONSTRUKT_REFERENCE__
#define __KONSTRUKT_REFERENCE__


typedef int ReferenceCounter;

void InitReferenceCounter( ReferenceCounter* counter );
void FreeReferenceCounter( ReferenceCounter* counter );

/**
 * Puts a reference on a reference counter.
 */
void Reference( ReferenceCounter* counter );

/**
 * Removes a reference from a reference counter.
 *
 * May not be called if the counter has no references left.
 */
void Release( ReferenceCounter* counter );

/**
 * Checks if a counter has references.
 */
bool HasReferences( ReferenceCounter* counter );


#endif
