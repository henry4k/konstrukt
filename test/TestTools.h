#ifndef __TEST_TOOLS_H__
#define __TEST_TOOLS_H__

#include <dummy/core.h>
#include <dummy/signal_sandbox.h>
#include <dummy/exception_sandbox.h>
#include <dummy/require.h>
#include <dummy/inline.h>
#include <dummy/bdd.hpp>

#define InlineTest DUMMY_INLINE_TEST
#define Require dummyRequire

using namespace dummy;

void InitTests();
int RunTests();

#endif
