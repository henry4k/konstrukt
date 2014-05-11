#ifndef __DUMMY_INLINE_H__
#define __DUMMY_INLINE_H__

#include "core.h"

/**
 * Adds all defined inline tests to the context.
 */
void dummyAddInlineTests();

/**
 * Defines an inline test.
 *
 * Use it like that:
 * @code
 * DUMMY_INLINE_TEST("This is an example test.", dummySignalSandbox)
 * {
 *     // Test code goes here.
 * }
 * @endcode
 */
#define DUMMY_INLINE_TEST( Name, Sandbox ) DUMMY_INLINE_TEST_(Name, Sandbox)


// --- implementation ---

int dummyAddInlineTest_( const char* name, dummySandbox sandbox, dummySandboxableFunction fn );

#define DUMMY_INLINE_TEST_CONCAT_HELPER_( A, B, C ) A ## B ## C
#define DUMMY_INLINE_TEST_CONCAT_( A, B, C ) DUMMY_INLINE_TEST_CONCAT_HELPER_(A, B, C)

#define DUMMY_INLINE_TEST_( Name, Sandbox ) \
    static void DUMMY_INLINE_TEST_CONCAT_(test_at_line_, __LINE__,) (); \
    static int DUMMY_INLINE_TEST_CONCAT_(test_at_line_, __LINE__, _registered) \
        = dummyAddInlineTest_(Name, Sandbox, DUMMY_INLINE_TEST_CONCAT_(test_at_line_, __LINE__,)); \
    static void DUMMY_INLINE_TEST_CONCAT_(test_at_line_, __LINE__,) ()

#endif
