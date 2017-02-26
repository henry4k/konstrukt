#include <assert.h>
#include <string.h> // memcpy, strlen, strcmp
#include <vector>

extern "C"
{
#define LUA_LIB
#include <lualib.h>
#include <lua_cjson.h>
}

#include "Common.h"
#include "Profiler.h"
#include "JobManager.h"
#include "Vfs.h"
#include "Lua.h"


static const int MAX_LUA_EVENT_NAME_SIZE = 32;


struct LuaEvent
{
    char name[MAX_LUA_EVENT_NAME_SIZE];
    int callbackReference;
};


DefineCounter(MemoryCounter, "memory", BYTE_COUNTER);
static lua_State* g_LuaState = NULL;
static std::vector<LuaEvent> g_LuaEvents;
static int g_LuaErrorFunction = LUA_NOREF;
static int g_LuaFunctionTable = LUA_NOREF;
static int g_LuaShutdownEvent = INVALID_LUA_EVENT;
static bool g_LuaRunning = false;
static JobId LuaUpdateJob;

static int Lua_SetErrorFunction( lua_State* l );
static int Lua_SetEventCallback( lua_State* l );
static int Lua_ErrorProxy( lua_State* l );
static int Lua_Log( lua_State* l );


void InitLua()
{
    assert(g_LuaState == NULL);
    assert(g_LuaEvents.empty());
    InitCounter(MemoryCounter);

    LogInfo("Compiled with " LUA_COPYRIGHT);
    const int version = (int)*lua_version(NULL);
    const int major = version/100;
    const int minor = (version-major*100)/10;
    const int patch = version-(major*100 + minor*10);
    LogInfo("Using Lua %d.%d.%d", major, minor, patch);

    lua_State* l = g_LuaState = luaL_newstate();
    g_LuaRunning = false;
    g_LuaEvents.clear();

    lua_gc(l, LUA_GCSTOP, 0); // only collect manually

    // Load standard modules (except package and io module)
    luaopen_base(l);
    luaL_requiref(l, LUA_COLIBNAME,   luaopen_coroutine, true);
    luaL_requiref(l, LUA_TABLIBNAME,  luaopen_table,     true);
    luaL_requiref(l, LUA_STRLIBNAME,  luaopen_string,    true);
    luaL_requiref(l, LUA_BITLIBNAME,  luaopen_bit32,     true);
    luaL_requiref(l, LUA_MATHLIBNAME, luaopen_math,      true);
    luaL_requiref(l, LUA_DBLIBNAME,   luaopen_debug,     true);
    luaL_requiref(l, "cjson",         luaopen_cjson,     true);

    lua_createtable(l, 0, 0);
    g_LuaFunctionTable = luaL_ref(l, LUA_REGISTRYINDEX);

    lua_rawgeti(l, LUA_REGISTRYINDEX, g_LuaFunctionTable);
    lua_setglobal(l, "_engine");

    RegisterFunctionInLua("SetErrorFunction", Lua_SetErrorFunction);
    RegisterFunctionInLua("SetEventCallback", Lua_SetEventCallback);
    RegisterFunctionInLua("Log", Lua_Log);

    g_LuaShutdownEvent = RegisterLuaEvent("Shutdown");
}

void DestroyLua()
{
    assert(g_LuaState);

    FireLuaEvent(g_LuaState, g_LuaShutdownEvent, 0, false);

    luaL_unref(g_LuaState, LUA_REGISTRYINDEX, g_LuaErrorFunction);
    luaL_unref(g_LuaState, LUA_REGISTRYINDEX, g_LuaFunctionTable);

    lua_close(g_LuaState);
    g_LuaState = NULL;
    g_LuaRunning = false;
    g_LuaEvents.clear();
}

lua_State* GetLuaState()
{
    return g_LuaState;
}

bool IsLuaRunning()
{
    return g_LuaRunning;
}

static int GetLuaMemoryInBytes()
{
    assert(g_LuaState);
    return lua_gc(g_LuaState, LUA_GCCOUNT, 0)*1024 +
           lua_gc(g_LuaState, LUA_GCCOUNTB, 0);
}

static void UpdateLua( void* data )
{
    ProfileScope("Lua GC");

    //const int memBeforeGC = GetLuaMemoryInBytes();
    lua_gc(g_LuaState, LUA_GCCOLLECT, 0);
    const int memAfterGC = GetLuaMemoryInBytes();
    SetCounter(MemoryCounter, memAfterGC);

    //const int delta = memBeforeGC - memAfterGC;
    //if(delta > 0)
    //    LogInfo("LUA GC UPDATE: %d bytes in use. %d bytes collected.",
    //            memAfterGC, delta);
}

void BeginLuaUpdate( JobManager* jobManager )
{
    LuaUpdateJob = CreateJob(jobManager, {"UpdateLua", UpdateLua});
}

void CompleteLuaUpdate( JobManager* jobManager )
{
    WaitForJobs(jobManager, &LuaUpdateJob, 1);
}

void RegisterFunctionInLua( const char* name, lua_CFunction fn )
{
    lua_rawgeti(g_LuaState, LUA_REGISTRYINDEX, g_LuaFunctionTable);
    lua_pushcfunction(g_LuaState, fn);
    lua_setfield(g_LuaState, -2, name);
    lua_pop(g_LuaState, 1);
}

void RegisterUserDataTypeInLua( const char* name, lua_CFunction gcCallback )
{
    lua_State* l = g_LuaState;

    const bool newTable = luaL_newmetatable(l, name);
    assert(newTable);

    if(gcCallback)
    {
        // set __gc callback
        lua_pushcfunction(l, gcCallback);
        lua_setfield(l, -2, "__gc");
    }

    // pop metatable
    lua_pop(l, 1);
}

void* PushUserDataToLua( lua_State* l, const char* typeName, int size )
{
    void* data = lua_newuserdata(l, size);

    luaL_getmetatable(l, typeName);
    if(!lua_istable(l, -1))
        FatalError("Lua userdata type '%s' does not exist.", typeName);

    lua_setmetatable(l, -2);
    return data;
}

void CopyUserDataToLua( lua_State* l, const char* typeName, int size, const void* data )
{
    void* luaData = PushUserDataToLua(l, typeName, size);
    memcpy(luaData, data, size);
}

void* GetUserDataFromLua( lua_State* l, int stackPosition, const char* typeName )
{
    void* data = lua_touserdata(l, stackPosition);
    if(!data)
        return NULL;

    const bool dataHasMetatable = lua_getmetatable(l, stackPosition);
    if(!dataHasMetatable)
        FatalError("no metatable attached");

    luaL_getmetatable(l, typeName);
    if(!lua_istable(l, -1) || !lua_rawequal(l, -2, -1))
        FatalError("type unknown");

    // pop both metatables
    lua_pop(l, 2);
    return data;
}

void* CheckUserDataFromLua( lua_State* l, int stackPosition, const char* typeName )
{
    void* data = GetUserDataFromLua(l, stackPosition, typeName);
    if(data)
    {
        return data;
    }
    else
    {
        luaL_argerror(l, stackPosition, Format("not of type %s", typeName));
        return NULL;
    }
}

void PushPointerToLua( lua_State* l, void* pointer )
{
    if(pointer)
        lua_pushlightuserdata(l, pointer);
    else
        lua_pushnil(l);
}

void* GetPointerFromLua( lua_State* l, int stackPosition )
{
    return lua_touserdata(l, stackPosition);
}

void* CheckPointerFromLua( lua_State* l, int stackPosition )
{
    void* pointer = GetPointerFromLua(l, stackPosition);
    if(pointer)
    {
        return pointer;
    }
    else
    {
        luaL_argerror(l, stackPosition, "was null or not a user pointer");
        return NULL;
    }
}

static int Lua_SetErrorFunction( lua_State* l )
{
    luaL_checktype(l, 1, LUA_TFUNCTION);
    lua_pushvalue(l, 1); // duplicate callback, because luaL_ref pops it
    const int old = g_LuaErrorFunction;
    g_LuaErrorFunction = luaL_ref(l, LUA_REGISTRYINDEX);

    if((old != LUA_NOREF) &&
       (old != LUA_REFNIL))
    {
        lua_rawgeti(l, LUA_REGISTRYINDEX, old);
        luaL_unref(l, LUA_REGISTRYINDEX, old);
        return 1;
    }
    else
    {
        return 0;
    }
}

static int Lua_ErrorProxy( lua_State* l )
{
    if((g_LuaErrorFunction != LUA_NOREF) &&
       (g_LuaErrorFunction != LUA_REFNIL))
    {
        lua_rawgeti(l, LUA_REGISTRYINDEX, g_LuaErrorFunction);
        lua_pushvalue(l, 1);
        lua_call(l, 1, 1);
    }
    else
    {
        const char* message = lua_tostring(l, 1);
        lua_pushfstring(l, "(Via fallback error handler:) %s", message);
    }
    return 1;
}

static void HandleLuaCallResult( lua_State* l, int result )
{
    switch(result)
    {
        case LUA_ERRRUN:
            {
                const char* message = lua_tostring(l, -1);
                FatalError("%s", message);
                lua_pop(l, 1);
            }

        case LUA_ERRMEM:
            FatalError("Lua encountered a memory allocation error.");

        case LUA_ERRERR:
            FatalError("Lua encountered an error while executing the error function.");

        case 0:
            return;

        default:
            FatalError("Unknown call result.");
    }
}

static const int LOAD_BUFFER_SIZE = 1024;
struct LoadState
{
    VfsFile* file;
    char buffer[LOAD_BUFFER_SIZE];
};

static const char* ReadLuaChunk( lua_State* l, void* userData, size_t* bytesRead )
{
    LoadState* state = (LoadState*)userData;
    *bytesRead = ReadVfsFile(state->file, state->buffer, LOAD_BUFFER_SIZE);
    return state->buffer;
}

void RunLuaScript( lua_State* l, const char* vfsPath )
{
    LogNotice("Running %s ...", vfsPath);

    lua_pushcfunction(l, Lua_ErrorProxy);

    VfsFile* file = OpenVfsFile(vfsPath, VFS_OPEN_READ);
    LoadState state;
    state.file = file;
    if(lua_load(l, ReadLuaChunk, &state, vfsPath, "t") == LUA_OK)
    {
        g_LuaRunning = true;
        const int callResult = lua_pcall(l, 0, 0, -2);
        g_LuaRunning = false;
        HandleLuaCallResult(l, callResult);
    }
    else
    {
        FatalError("%s", lua_tostring(l, -1));
        lua_pop(l, 2); // pop error function and error message
    }
    CloseVfsFile(file);
}

static int FindLuaEventByName( const char* name )
{
    assert(name != NULL);
    for(size_t i = 0; i < g_LuaEvents.size(); i++)
        if(strncmp(name, g_LuaEvents[i].name, MAX_LUA_EVENT_NAME_SIZE-1) == 0)
            return i;
    return -1;
}

int RegisterLuaEvent( const char* name )
{
    assert(FindLuaEventByName(name) == -1);
    assert(strlen(name) < MAX_LUA_EVENT_NAME_SIZE-1);

    LuaEvent event;
    memset(&event, 0, sizeof(event));
    CopyString(name, event.name, sizeof(event.name));
    event.callbackReference = LUA_NOREF;

    g_LuaEvents.push_back(event);

    const int id = (int)g_LuaEvents.size()-1;
    if(id < 0)
        FatalError("RegisterLuaEvent");

    return id;
}

int FireLuaEvent( lua_State* l, int id, int argumentCount, bool pushReturnValues )
{
    assert(id >= 0 && id < (int)g_LuaEvents.size());
    assert(argumentCount >= 0);

    LuaEvent* event = &g_LuaEvents[id];

    if((event->callbackReference == LUA_NOREF) ||
       (event->callbackReference == LUA_REFNIL))
    {
        lua_pop(l, argumentCount);
        return 0;
    }

    lua_pushcfunction(l, Lua_ErrorProxy);
    lua_insert(l, -(argumentCount+1)); // Move before arguments

    lua_rawgeti(l, LUA_REGISTRYINDEX, event->callbackReference);
    lua_insert(l, -(argumentCount+1)); // Move before arguments

    // The stack should now look like this:
    // error function
    // callback
    // arg 1
    // ...
    // arg n

    const int stackSizeBeforeCall = lua_gettop(l);
    g_LuaRunning = true;
    const int callResult = lua_pcall(
        l, argumentCount,
        pushReturnValues ? LUA_MULTRET : 0, // return value count
        -(argumentCount+2) // error func
    );
    g_LuaRunning = false;
    const int stackSizeAfterCall = lua_gettop(l);

    const int poppedValueCount = argumentCount + 1;
    const int returnValueCount =
        stackSizeAfterCall - (stackSizeBeforeCall-poppedValueCount);
    lua_remove(l, -(returnValueCount+1)); // Remove error function

    HandleLuaCallResult(l, callResult);

    return returnValueCount;
}

static int Lua_SetEventCallback( lua_State* l )
{
    const char* name = luaL_checkstring(l, 1);
    const int id = FindLuaEventByName(name);
    if(id < 0)
        return luaL_error(l, "No event called '%s' exists.", name);

    luaL_checktype(l, 2, LUA_TFUNCTION);
    lua_pushvalue(l, 2); // duplicate callback, because luaL_ref pops it
    g_LuaEvents[id].callbackReference = luaL_ref(l, LUA_REGISTRYINDEX);
    return 0;
}

static int Lua_Log( lua_State* l )
{
    static const char* levelNames[] =
    {
        "info",
        "notice",
        "warning",
        "error",
        "fatal error"
    };
    const LogLevel level = (LogLevel)luaL_checkoption(l, 1, NULL, levelNames);
    const char* message = luaL_checkstring(l, 2);
    Log(level, "%s", message);
    return 0;
}
