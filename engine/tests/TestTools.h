#ifndef __KONSTRUKT_TEST_TOOLS__
#define __KONSTRUKT_TEST_TOOLS__

#include <dummy/core.h>
#include <dummy/signal_sandbox.h>
#include <dummy/require.h>
#include <dummy/inline.hpp>

#define Require dummyRequire
#define AddTest(Name, Function) dummyAddTest(Name, dummySignalSandbox, Function)
#define InlineTest(Name) DUMMY_INLINE_TEST(Name, dummySignalSandbox)

void InitTests( int argc, char const * const * argv );
void InitTestVfs( const char* argv0 );
void InitTestJobManager();
int RunTests();

#endif
