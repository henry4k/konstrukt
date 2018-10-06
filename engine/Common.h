#ifndef __KONSTRUKT_COMMON__
#define __KONSTRUKT_COMMON__

#include <string.h> // memset
#include <stdbool.h>


// --- General ---

void InitCommon();
void DestroyCommon();

bool InSerialPhase();


// --- Memory allocation ---

void* Alloc( size_t size );
void  Free( void* pointer );
void* Realloc( void* oldPointer, size_t size );
#define AllocZeroed(size) memset(Alloc(size), 0, (size))

#define NEW(S) (S*)AllocZeroed(sizeof(S))
#define DELETE(P) do { memset((P), 0, sizeof(*(P))); Free(P); } while(0)

#define NEW_ARRAY(S,C) (S*)AllocZeroed(sizeof(S)*(C))
#define DELETE_ARRAY(P,C) do { memset((P), 0, sizeof(*(P))*(C)); Free(P); } while(0)


// --- Strings ---

/**
 * Generates a string according to the given format by using a buffer provided
 * by the user.
 *
 * The generated string always ends with a zero byte.  If the string would
 * exceed the buffer length it shortened accordingly.
 *
 * @return Length of the generated string without the terminating zero byte.
 */
int FormatBuffer( char* buffer, int size, const char* format, ... );

/**
 * Generates a string according to the given format by using an internal
 * buffer.
 *
 * @return A pointer to the formatted string.  Its valid till the next
 * invocation of this function.
 *
 * @note Currently this is limited to the main thread.
 */
const char* Format( const char* format, ... );

/**
 * Copies at most `destinationSize-1` bytes from `source` to `destination`
 * and ensures that the next byte is a `\0` byte.  Returns `false` if not all
 * bytes could be copied from `source`.
 */
bool CopyString( const char* source, char* destination, int destinationSize );

/**
 * If the given `prefix` string matches at the beginning of `value` then
 * a pointer to the characters after the matched prefix is returned -
 * otherwise `NULL` is returned.
 */
const char* MatchPrefix( const char* prefix, const char* value );


// --- Logging ---

enum LogLevel
{
    /**
     * Normal operational messages that require no action.
     */
    LOG_INFO,

    /**
     * Events that are unusual, but not error conditions.
     *
     * Interesting runtime events (startup/shutdown).
     */
    LOG_NOTICE,

    /**
     * May indicate that an error will occur if action is not taken.
     */
    LOG_WARNING,

    /**
     * Runtime errors or unexpected conditions.
     */
    LOG_ERROR,

    /**
     * A serious error occured - program will usually end soon.
     */
    LOG_FATAL_ERROR
};

typedef void (*LogHandler)( LogLevel level, const char* line );


void SetLogHandler( LogHandler handler );
LogHandler GetLogHandler();

void Log( LogLevel level, const char* format, ... );

#define LogInfo(...)    Log(LOG_INFO,    __VA_ARGS__)
#define LogNotice(...)  Log(LOG_NOTICE,  __VA_ARGS__)
#define LogWarning(...) Log(LOG_WARNING, __VA_ARGS__)
#define LogError(...)   Log(LOG_ERROR,   __VA_ARGS__)

bool PostConfigInitLog();


// --- Other utilities ---

/**
 * Will abort the program after emitting the log message.
 */
void FatalError( const char* format, ... );

typedef void (*FatalErrorHandlerFn)();

/**
 * Similar to `atexit` this registers a function which will be called upon
 * fatal errors (SIGABORT, etc.).
 * Multiple functions can be registered this way.
 */
void OnFatalError(FatalErrorHandlerFn fn);

#define Ensure( test ) do { if(!(test)) FatalError("Expression failed: %s", #test); } while(0)

#define REPEAT(N,I) for(int _n = (int)(N), I = 0; I < _n; I++)

#if !defined(FIELD_SIZE)
#define FIELD_SIZE(container, field) sizeof(((container*)0)->field)
#endif

#define CONCAT_(a, b) a ## b
#define CONCAT(a, b) CONCAT_(a,b)

// TODO: Alternatively use dbgtools/static_assert.h!
#if !defined(__cplusplus) && defined(__USE_ISOC11)
    #define static_assert _Static_assert
#elif defined(__cplusplus) && __cplusplus >= 201103L
    // static_assert is provided by C++11
#else
    #define static_assert( test, message ) \
        typedef struct { int value: !!(test); } static_assert_##__COUNTER__
#endif

#endif
