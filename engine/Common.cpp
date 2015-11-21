#if defined(__WINDOWS__)
    #define WIN32_LEAN_AND_MEAN
    #define NOGDI
    #include <windows.h> // GetFileAttributes
#else
    #include <sys/stat.h> // stat
    #include <signal.h>
    #include <unistd.h> // isatty
#endif
#include <string.h> // strncpy, strcmp
#include <stdlib.h> // exit
#include <stdio.h> // printf
#include <stdarg.h>
#include <assert.h>
#include "Config.h"
#include "Common.h"

const char* Format( const char* format, ... )
{
    static char buffer[512];

    va_list vl;
    va_start(vl, format);
        vsprintf(buffer, format, vl);
    va_end(vl);

    return buffer;
}

static void SimpleLogHandler( LogLevel level, const char* line )
{
    const char* prefix = "";
    const char* postfix = "";
    FILE* file = NULL;
    switch(level)
    {
        case LOG_INFO:
            file = stdout;
            break;

        case LOG_ERROR:
            prefix = "ERROR: ";
            file = stderr;
            break;

        case LOG_FATAL_ERROR:
            prefix = "FATAL ERROR: ";
            file = stderr;
            break;
    }

    fprintf(file, "%s%s%s\n", prefix, line, postfix);
}

static void ColorLogHandler( LogLevel level, const char* line )
{
    const char* prefix = "";
    const char* postfix = "";
    FILE* file = NULL;
    switch(level)
    {
        case LOG_INFO:
            file = stdout;
            break;

        case LOG_ERROR:
            prefix = "\033[31mERROR: ";
            postfix = "\033[0m";
            file = stderr;
            break;

        case LOG_FATAL_ERROR:
            prefix = "\033[31;1mFATAL ERROR: ";
            postfix = "\033[0m";
            file = stderr;
            break;
    }

    fprintf(file, "%s%s%s\n", prefix, line, postfix);
}

static LogHandler g_LogHandler = SimpleLogHandler;

void SetLogHandler( LogHandler handler )
{
    assert(handler != NULL);
    g_LogHandler = handler;
}

LogHandler GetLogHandler()
{
    return g_LogHandler;
}

void LogV( LogLevel level, const char* format, va_list vl )
{
    static char buffer[512];
    vsprintf(buffer, format, vl);

    const char* start = buffer;
    for(char* current = buffer; ; ++current)
    {
        if(*current == '\n')
        {
            *current = '\0';
            g_LogHandler(level, start);
            start = current+1;
        }
        else if(*current == '\0')
        {
            g_LogHandler(level, start);
            break;
        }
    }
}

void Log( const char* format, ... )
{
    va_list vl;
    va_start(vl, format);
    LogV(LOG_INFO, format, vl);
    va_end(vl);
}

void Error( const char* format, ... )
{
    va_list vl;
    va_start(vl, format);
    LogV(LOG_ERROR, format, vl);
    va_end(vl);
}

void FatalError( const char* format, ... )
{
    va_list vl;
    va_start(vl, format);
    LogV(LOG_ERROR, format, vl);
    va_end(vl);
    //raise(SIGTRAP);
    exit(EXIT_FAILURE);
}

#if defined(__WINDOWS__)
static LogHandler AutodetectLogHandler()
{
    return SimpleLogHandler;
}
#else
static LogHandler AutodetectLogHandler()
{
    if(isatty(fileno(stdout)) && isatty(fileno(stderr)))
        return ColorLogHandler;
    else
        return SimpleLogHandler;
}
#endif

bool PostConfigInitLog()
{
    const char* handlerName = GetConfigString("debug.log-handler", "auto");
    LogHandler handler = NULL;

    if(strcmp(handlerName, "simple") == 0)
        handler = SimpleLogHandler;
    else if(strcmp(handlerName, "color") == 0)
        handler = ColorLogHandler;
    else if(strcmp(handlerName, "auto") == 0)
        handler = AutodetectLogHandler();

    if(handler)
    {
        SetLogHandler(handler);
        return true;
    }
    else
    {
        Error("Unknown log handler '%s'.", handlerName);
        return false;
    }
}

bool CopyString( const char* source, char* destination, int destinationSize )
{
    assert(source && destination && destinationSize > 0);

    strncpy(destination, source, destinationSize);
    if(destination[destinationSize-1] == '\0')
    {
        return true;
    }
    else
    {
        destination[destinationSize-1] = '\0';
        return false;
    }
}

#if defined(__WINDOWS__)
FileType GetFileType( const char* path )
{
    DWORD type = GetFileAttributesA(path);
    if(type == INVALID_FILE_ATTRIBUTES)
        return FILE_TYPE_INVALID;

    if(type & FILE_ATTRIBUTE_DIRECTORY)
        return FILE_TYPE_DIRECTORY;
    else
        return FILE_TYPE_REGULAR;
}
#else
FileType GetFileType( const char* path )
{
    struct stat info;
    if(stat(path, &info) == -1)
        return FILE_TYPE_INVALID;
    if(info.st_mode & S_IFREG)
        return FILE_TYPE_REGULAR;
    else if(info.st_mode & S_IFDIR)
        return FILE_TYPE_DIRECTORY;
    else
        return FILE_TYPE_UNKNOWN;
}
#endif
