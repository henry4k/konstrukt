#ifndef __DUMMY_SIGNAL_SANDBOX_H__
#define __DUMMY_SIGNAL_SANDBOX_H__

#include "core.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Run `fn` in a sandbox, that catches signals.
 * @see dummySandbox
 */
int dummySignalSandbox( dummySandboxableFunction fn, const char** abortReason );

#ifdef __cplusplus
}
#endif

#endif
