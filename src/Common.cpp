#if !defined(__WIN32__)
    #include <signal.h>
#endif
#include <string.h>
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

enum LogLevel
{
    LOG_INFO,
    LOG_ERROR,
    LOG_FATAL_ERROR
};

void LogV( LogLevel level, const char* format, va_list vl )
{
    const char* prefix = "";
    const char* postfix = "";
    FILE* file = NULL;
    switch(level)
    {
        case LOG_INFO:
            //prefix = "";
            //postfix = "";
            file = stdout;
            break;

        case LOG_ERROR:
            //prefix = "\033[31mERROR: ";
            //postfix = "\033[0m";
            file = stderr;
            break;

        case LOG_FATAL_ERROR:
            //prefix = "\033[31;1mFATAL ERROR: ";
            //postfix = "\033[0m";
            file = stderr;
            break;
    }

    static char buffer[512];
    vsprintf(buffer, format, vl);

    const char* start = buffer;
    for(char* current = buffer; ; ++current)
    {
        if(*current == '\n')
        {
            *current = '\0';
            fprintf(file, "%s%s%s\n", prefix, start, postfix);
            start = current+1;
        }
        else if(*current == '\0')
        {
            if(current-start > 1) // Skip empty lines.
                fprintf(file, "%s%s%s\n", prefix, start, postfix);
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

#if defined(__WIN32__)
    abort();
#else
    raise(SIGTRAP);
#endif
}
