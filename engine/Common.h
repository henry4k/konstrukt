#ifndef __KONSTRUKT_COMMON__
#define __KONSTRUKT_COMMON__

#include <stdlib.h> // calloc, free
#include <string.h> // memset

//#define REPEAT(N,I) for(int n = (N), I = 0; I < n; I++)
#define REPEAT(N,I) for(int I = 0; I < (N); I++)
#define NEW(S) (S*)calloc(1, sizeof(S))
#define DELETE(S,P) { memset(P, 0, sizeof(S)); free(P); }

enum LogLevel
{
    LOG_INFO,
    LOG_ERROR,
    LOG_FATAL_ERROR
};

typedef void (*LogHandler)( LogLevel level, const char* line );


int FormatBuffer( char* buffer, int size, const char* format, ... );
const char* Format( const char* format, ... );

void SetLogHandler( LogHandler handler );
LogHandler GetLogHandler();

void Log( const char* format, ... );
void Error( const char* format, ... );

/**
 * Will abort the program after emitting the log message.
 */
void FatalError( const char* format, ... );

bool PostConfigInitLog();

/**
 * Copies at most `destinationSize-1` bytes from `source` to `destination`
 * and ensures that the next byte is a `\0` byte.  Returns `false` if not all
 * bytes could be copied from `source`.
 */
bool CopyString( const char* source, char* destination, int destinationSize );

#endif
