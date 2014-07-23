#ifndef __APOAPSIS_TEST_TOOLS__
#define __APOAPSIS_TEST_TOOLS__

#include <dummy/core.h>
#include <dummy/signal_sandbox.h>
#include <dummy/exception_sandbox.h>
#include <dummy/require.h>
#include <dummy/inline.h>
#include <dummy/bdd.hpp>

#define InlineTest DUMMY_INLINE_TEST
#define Require dummyRequire

using namespace dummy;

void InitTests( int argc, char const * const * argv );
int RunTests();

#endif
