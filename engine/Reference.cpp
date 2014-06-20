#include <assert.h>

#include "Reference.h"


void InitReferenceCounter( ReferenceCounter* counter )
{
    *counter = 0;
}

void FreeReferenceCounter( ReferenceCounter* counter )
{
    // not needed
}

void Reference( ReferenceCounter* counter )
{
    *counter++;
}

void Release( ReferenceCounter* counter )
{
    assert(*counter > 0);
    *counter--;
}

bool HasReferences( ReferenceCounter* counter )
{
    return *counter > 0;
}