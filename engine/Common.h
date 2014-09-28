#ifndef __APOAPSIS_COMMON__
#define __APOAPSIS_COMMON__

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <assert.h>

const char* Format( const char* format, ... );

enum LogLevel
{
    LOG_INFO,
    LOG_ERROR,
    LOG_FATAL_ERROR
};

typedef void (*LogHandler)( LogLevel level, const char* line );
void DefaultLogHandler( LogLevel level, const char* line );
void SetLogHandler( LogHandler handler );
LogHandler GetLogHandler();

void LogV( LogLevel level, const char* format, va_list vl );

void Log( const char* format, ... );
void Error( const char* format, ... );

/**
 * Will abort the program after emitting the log message.
 */
void FatalError( const char* format, ... );

/**
 * Copies at most `destinationSize-1` bytes from `source` to `destination`
 * and ensures that the next byte is a `\0` byte.  Returns `false` if not all
 * bytes could be copied from `source`.
 */
bool CopyString( const char* source, char* destination, int destinationSize );

enum FileType
{
    FILE_TYPE_INVALID,
    FILE_TYPE_UNKNOWN,
    FILE_TYPE_REGULAR,
    FILE_TYPE_DIRECTORY
};

FileType GetFileType( const char* path );

#endif
