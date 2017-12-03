#ifndef __DUMMY_REQUIRE_H__
#define __DUMMY_REQUIRE_H__

#include <string.h> // strcmp

#include "core.h"


#define dummyRequire( expression ) \
        dummyRequire_((expression), #expression, \
                      __FILE__, __LINE__)

#define dummyRequireIntEquals( expected, actual ) \
        dummyRequireIntEquals_((expected), (actual), #actual, \
                               __FILE__, __LINE__)

#define dummyRequireFloatEquals( expected, tolerance, actual ) \
        dummyRequireFloatEquals_((expected), (tolerance), (actual), #actual, \
                                 __FILE__, __LINE__)

#define dummyRequireStringEquals( expected, actual ) \
        dummyRequireStringEquals_((expected), (actual), #actual, \
                                  __FILE__, __LINE__)

#define dummyRequirePointerEquals( expected, actual ) \
        dummyRequirePointerEquals_((expected), #expected, (actual), #actual, \
                                   __FILE__, __LINE__)


static inline void dummyRequire_( int result,
                                  const char* expression,
                                  const char* file, int line )
{
    if(!result)
        dummyAbortTest(DUMMY_FAIL_TEST,
            "In %s:%d:\n"
            "%s failed",
            file, line,
            expression);
}

static inline void dummyRequireIntEquals_( long expected,
                                           long actual,
                                           const char* actualRepresentation,
                                           const char* file, int line )
{
    if(expected != actual)
        dummyAbortTest(DUMMY_FAIL_TEST,
            "In %s:%d:\n"
            "    %d == %s failed\n"
            "was %d == %d",
            file, line,
            expected, actualRepresentation,
            expected, actual);
}

static inline void dummyRequireFloatEquals_( double expected,
                                             double tolerance,
                                             double actual,
                                             const char* actualRepresentation,
                                             const char* file, int line )
{
    if(actual > expected+tolerance ||
       actual < expected-tolerance)
    {
        dummyAbortTest(DUMMY_FAIL_TEST,
            "In %s:%d:\n"
            "    %g+-%g == %s failed\n"
            "was %g+-%g == %g",
            file, line,
            expected, tolerance, actualRepresentation,
            expected, tolerance, actual);
    }
}

static inline void dummyRequireStringEquals_( const char* expected,
                                              const char* actual,
                                              const char* actualRepresentation,
                                              const char* file, int line )
{
    if(strcmp(expected, actual) != 0)
        dummyAbortTest(DUMMY_FAIL_TEST,
            "In %s:%d:\n"
            "    \"%s\" == %s failed\n"
            "was \"%s\" == \"%s\"",
            file, line,
            expected, actualRepresentation,
            expected, actual);
}

static inline void dummyRequirePointerEquals_( const void* expected,
                                               const char* expectedRepresentation,
                                               const void* actual,
                                               const char* actualRepresentation,
                                               const char* file, int line )
{
    if(expected != actual)
        dummyAbortTest(DUMMY_FAIL_TEST,
            "In %s:%d:\n"
            "    %s == %s failed\n"
            "was %p == %p",
            file, line,
            expectedRepresentation, actualRepresentation,
            expected, actual);
}


#endif
