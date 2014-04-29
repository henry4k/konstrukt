#ifndef __DUMMY_XML_REPORTER_H__
#define __DUMMY_XML_REPORTER_H__

#include "core.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Reports in a JUnit compatible XML format.
 */
const dummyReporter* dummyGetXMLReporter();

#ifdef __cplusplus
}
#endif

#endif
