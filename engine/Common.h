#ifndef __KONSTRUKT_COMMON__
#define __KONSTRUKT_COMMON__

enum LogLevel
{
    LOG_INFO,
    LOG_ERROR,
    LOG_FATAL_ERROR
};

enum FileType
{
    FILE_TYPE_INVALID,
    FILE_TYPE_UNKNOWN,
    FILE_TYPE_REGULAR,
    FILE_TYPE_DIRECTORY
};

typedef void (*LogHandler)( LogLevel level, const char* line );


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

FileType GetFileType( const char* path );

#endif
