#include <assert.h>

#include "Reference.h"


void InitReferenceCounter( ReferenceCounter* counter )
{
    *counter = 0;
}

void FreeReferenceCounter( ReferenceCounter* counter )
{
    assert(*counter == 0);
}

void Reference( ReferenceCounter* counter )
{
    *counter += 1;
}

void Release( ReferenceCounter* counter )
{
    assert(*counter > 0);
    *counter -= 1;
}

bool HasReferences( ReferenceCounter* counter )
{
    return *counter > 0;
}
