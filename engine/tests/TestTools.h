#ifndef __APOAPSIS_TEST_TOOLS__
#define __APOAPSIS_TEST_TOOLS__

#include <dummy/core.h>
#include <dummy/signal_sandbox.h>
#include <dummy/require.h>

#define Require dummyRequire
#define AddTest(Name, Function) dummyAddTest(Name, dummySignalSandbox, Function)

void InitTests( int argc, char const * const * argv );
int RunTests();

#endif
