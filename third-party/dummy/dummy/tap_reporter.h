#ifndef __DUMMY_TAP_REPORTER_H__
#define __DUMMY_TAP_REPORTER_H__

#include "core.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Reports using the 'dummy anything protocol' (TAP).
 */
const dummyReporter* dummyGetTAPReporter();

#ifdef __cplusplus
}
#endif

#endif
