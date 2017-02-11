#if !defined(_WIN32)
    #include <unistd.h> // isatty
#endif

#include <string.h> // strncpy, strcmp
#include <stdlib.h> // exit
#include <stdio.h> // printf
#include <stdarg.h>
#include <assert.h>

#include "Warnings.h"
BEGIN_EXTERNAL_CODE
#include <konstrukt_stb_sprintf.h>
END_EXTERNAL_CODE

#include "Config.h"
#include "Common.h"


// --- Strings ---

static int FormatBufferV( char* buffer, int size, const char* format, va_list va )
{
    const int length = stbsp_vsnprintf(buffer, size, format, va);
    if(length >= size)
        FatalError("Buffer size exceeded.");
    return length;
}

int FormatBuffer( char* buffer, int size, const char* format, ... )
{
    va_list vl;
    va_start(vl, format);
    const int length = FormatBufferV(buffer, size, format, vl);
    va_end(vl);
    return length;
}

const char* Format( const char* format, ... )
{
    static char buffer[512];
    va_list vl;
    va_start(vl, format);
    FormatBufferV(buffer, 512, format, vl);
    va_end(vl);
    return buffer;
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


// --- Logging ---

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

        case LOG_WARNING:
            prefix = "WARNING: ";
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

        case LOG_WARNING:
            prefix = "\033[31mWARNING: ";
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

void Warn( const char* format, ... )
{
    va_list vl;
    va_start(vl, format);
    LogV(LOG_WARNING, format, vl);
    va_end(vl);
}

void FatalError( const char* format, ... )
{
    va_list vl;
    va_start(vl, format);
    LogV(LOG_FATAL_ERROR, format, vl);
    va_end(vl);
    exit(EXIT_FAILURE);
}

#if defined(_WIN32)
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
        FatalError("Unknown log handler '%s'.", handlerName);
        return false;
    }
}
