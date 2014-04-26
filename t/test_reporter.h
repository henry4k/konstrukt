#ifndef __DUMMY_TEST_REPORTER_H__
#define __DUMMY_TEST_REPORTER_H__

#include "../dummy/core.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Reports as many information as possible using a custom format.
 *
 * @note
 * It is used for self dummying and not intended to be used externally,
 * since the format may change over time.
 */
const dummyReporter* dummyGetTestReporter();

#ifdef __cplusplus
}
#endif

#endif
