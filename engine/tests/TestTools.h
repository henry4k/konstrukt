#ifndef __KONSTRUKT_TEST_TOOLS__
#define __KONSTRUKT_TEST_TOOLS__

#include <dummy/core.h>
#include <dummy/signal_sandbox.h>
#include <dummy/require.h>

#define Require dummyRequire
#define AddTest(Name, Function) dummyAddTest(Name, dummySignalSandbox, Function)

void InitTests( int argc, char const * const * argv );
void InitTestVfs( const char* argv0 ); // call this if you need the VFS module in your test
int RunTests();

#endif
