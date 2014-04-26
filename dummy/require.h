#ifndef __DUMMY_REQUIRE_H__
#define __DUMMY_REQUIRE_H__

#include "core.h"

#define dummyRequire( E ) dummyRequire_(#E, (E))

static inline void dummyRequire_( const char* expression, int result )
{
    if(!result)
        dummyAbortTest(DUMMY_FAIL_TEST, "%s failed", expression);
}

#endif
