#ifndef __KONSTRUKT_COMMON__
#define __KONSTRUKT_COMMON__

#include <string.h> // memset


// --- Memory allocation ---

void* Alloc( size_t size );
void  Free( void* pointer );
void* ReAlloc( void* oldPointer, size_t size );
#define AllocZeroed(size) memset(Alloc(size), 0, (size))

#define NEW(S) (S*)AllocZeroed(sizeof(S))
#define DELETE(S,P) { memset(P, 0, sizeof(S)); Free(P); }


// --- Strings ---

int FormatBuffer( char* buffer, int size, const char* format, ... );
const char* Format( const char* format, ... );

/**
 * Copies at most `destinationSize-1` bytes from `source` to `destination`
 * and ensures that the next byte is a `\0` byte.  Returns `false` if not all
 * bytes could be copied from `source`.
 */
bool CopyString( const char* source, char* destination, int destinationSize );


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

/**
 * Will abort the program after emitting the log message.
 */
void FatalError( const char* format, ... );

bool PostConfigInitLog();


// --- Other utilities ---

#define REPEAT(N,I) for(int _n = (int)(N), I = 0; I < _n; I++)
//#define REPEAT(N,I) for(int I = 0; I < (N); I++)

#endif
