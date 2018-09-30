#include <assert.h>

#include "Common.h"
#include "Reference.h"


void InitReferenceCounter( ReferenceCounter* counter )
{
    assert(InSerialPhase());
    *counter = 0;
}

void FreeReferenceCounter( ReferenceCounter* counter )
{
    assert(InSerialPhase());
    assert(*counter == 0);
}

void Reference( ReferenceCounter* counter )
{
    assert(InSerialPhase());
    *counter += 1;
}

void Release( ReferenceCounter* counter )
{
    assert(InSerialPhase());
    assert(*counter > 0);
    *counter -= 1;
}

bool HasReferences( ReferenceCounter* counter )
{
    assert(InSerialPhase());
    return *counter > 0;
}
