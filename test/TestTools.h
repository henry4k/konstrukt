#ifndef __TEST_TOOLS_H__
#define __TEST_TOOLS_H__

#include <dummy/core.h>
#include <dummy/require.h>
#include <dummy/bdd.hpp>

#define Require dummyRequire

using namespace dummy;

void InitTests();
int RunTests();

#endif
