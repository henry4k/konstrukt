#ifndef __DUMMY_XML_REPORTER_H__
#define __DUMMY_XML_REPORTER_H__

#include <stdio.h> // FILE
#include "core.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Reports in a JUnit compatible XML format.
 *
 * Example:
 * @code
 * dummyGetXMLReporter(stdout, "    ", "\n")
 * @endcode
 *
 * @param file
 * The report is written to `file`.
 *
 * @param tab
 * Character sequence that is used as a tab for indentation.
 *
 * @param newline
 * Character sequence that is used to separate lines.
 */
const dummyReporter* dummyGetXMLReporter( FILE* file, const char* tab, const char* newline );

#ifdef __cplusplus
}
#endif

#endif
