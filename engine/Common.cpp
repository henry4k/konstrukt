#if !defined(_WIN32)
    #include <unistd.h> // isatty
#endif

#include <string.h> // strncpy, strcmp
#include <stdlib.h> // abort, exit, EXIT_FAILURE, malloc, realloc, free
#include <stdio.h> // printf
#include <signal.h> // signal, raise, SIG*
#include <stdarg.h>
#include <assert.h>

#include "Constants.h" // KONSTRUKT_STACKTRACE_ENABLED, KONSTRUKT_PROFILER_ENABLED
#include "Warnings.h"
BEGIN_EXTERNAL_CODE
#include <konstrukt_stb_sprintf.h>
#if defined(KONSTRUKT_STACKTRACE_ENABLED)
#include <dbgtools/callstack.h>
#endif
END_EXTERNAL_CODE

#include "Config.h"
#include "Lua.h"
#include "Profiler.h"
#include "Array.h"
#include "Common.h"


static const int STRING_BUFFER_SIZE = 1024;


// --- General ---

static void InitMemoryAllocator();
static void InitFatalErrorHandler();
static void DestroyFatalErrorHandler();

void InitCommon()
{
    InitMemoryAllocator();
    InitFatalErrorHandler();
}

void DestroyCommon()
{
    DestroyFatalErrorHandler();
}


// --- Memory allocation ---

#if defined(KONSTRUKT_PROFILER_ENABLED)
#define USE_ALLOCATION_INFO
#endif

#if defined(USE_ALLOCATION_INFO)
DefineCounter(MemoryAllocationCounter, "memory allocations");
DefineCounter(MemoryCounter, "memory", BYTE_COUNTER);

struct AllocationInfo
{
    size_t size;
};

static const size_t AllocationInfoSize = sizeof(AllocationInfo);

static void InitMemoryAllocator()
{
    InitCounter(MemoryAllocationCounter);
    InitCounter(MemoryCounter);
}

static void* GetAllocationPointer( void* data )
{
    if(data)
        return ((AllocationInfo*)data) - 1;
    else
        return NULL;
}

static void* GetDataPointer( void* data )
{
    return ((AllocationInfo*)data) + 1;
}

static void BeforeAllocationDisposal( void* allocation )
{
    if(allocation)
    {
        AllocationInfo* info = (AllocationInfo*)allocation;
        DecreaseCounter(MemoryAllocationCounter, 1);
        DecreaseCounter(MemoryCounter, info->size);
    }
}

static void AfterAllocationCreation( void* allocation, size_t size )
{
    AllocationInfo* info = (AllocationInfo*)allocation;
    info->size = size;
    IncreaseCounter(MemoryAllocationCounter, 1);
    IncreaseCounter(MemoryCounter, size);
}
#else
static const size_t AllocationInfoSize = 0;
static void InitMemoryAllocator() {}
static void* GetAllocationPointer( void* data ) { return data; }
static void* GetDataPointer( void* allocation ) { return allocation; }
static void BeforeAllocationDisposal( void* allocation ) {}
static void AfterAllocationCreation( void* allocation, size_t size ) {}
#endif

void Free( void* data )
{
    void* allocation = GetAllocationPointer(data);
    BeforeAllocationDisposal(allocation);
    free(allocation);
}

void* Realloc( void* oldData, size_t size )
{
    void* oldAllocation = GetAllocationPointer(oldData);
    BeforeAllocationDisposal(oldAllocation);
    void* newAllocation = realloc(oldAllocation, AllocationInfoSize+size);
    if(!newAllocation)
        FatalError("Memory allocation of %$.2d bytes failed.", size);
    AfterAllocationCreation(newAllocation, size);
    return GetDataPointer(newAllocation);
}

void* Alloc( size_t size )
{
    return Realloc(NULL, size);
}


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

static const char* FormatV( const char* format, va_list va )
{
    static char buffer[STRING_BUFFER_SIZE];
    FormatBufferV(buffer, STRING_BUFFER_SIZE, format, va);
    return buffer;
}

const char* Format( const char* format, ... )
{
    va_list vl;
    va_start(vl, format);
    const char* result = FormatV(format, vl);
    va_end(vl);
    return result;
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

const char* MatchPrefix( const char* prefix, const char* value )
{
    const size_t prefixLength = strlen(prefix);
    if(strncmp(prefix, value, prefixLength) == 0)
        return &value[prefixLength];
    else
        return NULL;
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
        case LOG_NOTICE:
            file = stdout;
            break;

        case LOG_WARNING:
            prefix = "WARNING: ";
            file = stderr;
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
            prefix = "\033[30;1m";
            postfix = "\033[0m";
            file = stdout;
            break;

        case LOG_NOTICE:
            file = stdout;
            break;

        case LOG_WARNING:
            prefix = "\033[33mWARNING: ";
            postfix = "\033[0m";
            file = stderr;
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
    static char buffer[STRING_BUFFER_SIZE];
    FormatBufferV(buffer, STRING_BUFFER_SIZE, format, vl);

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

void Log( LogLevel level, const char* format, ... )
{
    va_list vl;
    va_start(vl, format);
    LogV(level, format, vl);
    va_end(vl);
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


// --- Other utilities ---

#if defined(KONSTRUKT_STACKTRACE_ENABLED)
static void LogStackTrace( LogLevel level, int skippedFrames )
{
    static const int MAX_STACK_DEPTH = 64;
    Log(level, "engine stack traceback:");

    void* frames[MAX_STACK_DEPTH];
    const int frameCount = callstack(skippedFrames+1, frames, MAX_STACK_DEPTH);

    callstack_symbol_t symbols[MAX_STACK_DEPTH];
    char symbolBuffer[1024];

    const int symbolCount =
        callstack_symbols(frames, symbols, frameCount, symbolBuffer, sizeof(symbolBuffer));

    REPEAT(symbolCount, i)
    {
        if(symbols[i].line == 0) // if the location seems to be unavailable
            Log(level, "\tin %s", symbols[i].function);
        else
            Log(level, "\t%s:%d in %s",
                       symbols[i].file,
                       symbols[i].line,
                       symbols[i].function);
    }
}
#else
static void LogStackTrace( LogLevel level, int skippedFrames ) {}
#endif

void FatalError( const char* format, ... )
{
    va_list vl;
    va_start(vl, format);
    if(IsLuaRunning())
    {
        LogStackTrace(LOG_FATAL_ERROR, 1);
        const char* message = FormatV(format, vl);
        va_end(vl);
        lua_pushstring(GetLuaState(), message);
        lua_error(GetLuaState());
        // TODO: I'm not sure whether this is the right solution.
        // The intention is to make Lua print a stacktrace but this may
        // not stop the simulation loop.
    }
    else
    {
        LogV(LOG_FATAL_ERROR, format, vl);
        va_end(vl);
        LogStackTrace(LOG_FATAL_ERROR, 1);
        abort();
    }
}

static Array<FatalErrorHandlerFn> FatalErrorHandlers;

static void FatalErrorSignalHandler(int sig);

static void InitFatalErrorHandler()
{
    InitArray(&FatalErrorHandlers);
    signal(SIGABRT, FatalErrorSignalHandler);
    // SIGFPE, SIGILL, SIGSEGV ?
}

static void DestroyFatalErrorHandler()
{
    DestroyArray(&FatalErrorHandlers);
}

void OnFatalError(FatalErrorHandlerFn fn)
{
    AppendToArray(&FatalErrorHandlers, 1, &fn);
}

static void FatalErrorSignalHandler(int sig)
{
    REPEAT(FatalErrorHandlers.length, i)
    {
        FatalErrorHandlerFn fn = *GetArrayElement(&FatalErrorHandlers, i);
        fn();
    }
    ClearArray(&FatalErrorHandlers);
    exit(EXIT_FAILURE);
}
