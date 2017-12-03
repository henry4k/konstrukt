#ifndef __DUMMY_EXCEPTION_SANDBOX_HPP__
#define __DUMMY_EXCEPTION_SANDBOX_HPP__

#include "core.h"

/**
 * Run `fn` in a sandbox, that catches exceptions and signals.
 * @see dummySandbox
 */
int dummyExceptionSandbox( dummySandboxableFunction fn, const char** abortReason );

#endif
