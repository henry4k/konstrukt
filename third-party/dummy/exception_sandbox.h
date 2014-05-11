#ifndef __DUMMY_EXCEPTION_SANDBOX_H__
#define __DUMMY_EXCEPTION_SANDBOX_H__

#include "core.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Run `fn` in a sandbox, that catches exceptions and signals.
 * @see dummySandbox
 */
int dummyExceptionSandbox( dummySandboxableFunction fn, const char** abortReason );

#ifdef __cplusplus
}
#endif

#endif
