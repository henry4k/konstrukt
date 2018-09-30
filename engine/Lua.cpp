#include <assert.h>
#include <string.h> // memcpy, strlen, strcmp

extern "C"
{
#define LUA_LIB
#include <lualib.h>
#include <lua_cjson.h>
#include <lua_taggedcoro.h>
}

#include "Constants.h" // KONSTRUKT_PROFILER_ENABLED
#include "Common.h"
#include "Profiler.h"
#include "JobManager.h"
#include "Vfs.h"
#include "Array.h"
#include "Lua.h"


static const int MAX_JOB_NAME_SIZE = 32;
static const char* LUA_WORKER_KEY = "konstrukt_worker";


struct LuaTypeDescription
{
    const char* name;
    lua_CFunction gcCallback;
};

enum LuaFunctionFlags
{
    /**
     * The function is thread-safe - it can be called anytime, not just during
     * the serial phase.
     */
    LUA_THREAD_SAFE = (1 << 0),

    /**
     * The function won't return any results.
     */
    LUA_NO_RESULTS = (1 << 1)
};

struct LuaFunctionDescription
{
    const char* name;
    lua_CFunction function;
    int flags;
};

struct LuaWorker
{
    lua_State* state;
    int errorHandler;
    JobId job;
    char jobName[MAX_JOB_NAME_SIZE];
};


static LuaWorker* GetLuaWorkerFromState( lua_State* l );
static int Lua_SetErrorHandler( lua_State* l );
static int Lua_Log( lua_State* l );
static void PushLuaScript( lua_State* l, const char* vfsPath );
static int CallLuaFunction( lua_State* l, int argumentCount, int returnValueCount );


DefineCounter(MemoryCounter, "memory", BYTE_COUNTER);
static Array<LuaFunctionDescription> LuaFunctions;
static Array<LuaTypeDescription> LuaTypes;
static Array<LuaWorker> LuaWorkers;


// --- General ---

void InitLua()
{
    assert(InSerialPhase());

    InitCounter(MemoryCounter);

    LogInfo("Compiled with " LUA_COPYRIGHT);
    const int version = (int)*lua_version(NULL);
    const int major = version/100;
    const int minor = (version-major*100)/10;
    const int patch = version-(major*100 + minor*10);
    LogInfo("Using Lua %d.%d.%d", major, minor, patch);

    InitArray(&LuaFunctions);
    InitArray(&LuaTypes);
    InitArray(&LuaWorkers);

    REGISTER_LUA_FUNCTION(SetErrorHandler);
    REGISTER_LUA_FUNCTION(Log);
}

void DestroyLua()
{
    assert(InSerialPhase());

    SetLuaWorkerCount(0);

    DestroyArray(&LuaFunctions);
    DestroyArray(&LuaTypes);
    DestroyArray(&LuaWorkers);
}

void RegisterLuaFunction( const char* name, lua_CFunction fn )
{
    assert(InSerialPhase());
    assert(LuaWorkers.length == 0);
    LuaFunctionDescription* desc = AllocateAtEndOfArray(&LuaFunctions, 1);
    desc->name = name;
    desc->function = fn;
    desc->flags = 0;
}

void RegisterLuaType( const char* name, lua_CFunction gcCallback )
{
    assert(InSerialPhase());
    assert(LuaWorkers.length == 0);
    LuaTypeDescription* desc = AllocateAtEndOfArray(&LuaTypes, 1);
    desc->name = name;
    desc->gcCallback = gcCallback;
}

static int Lua_SetErrorHandler( lua_State* l )
{
    luaL_checktype(l, 1, LUA_TFUNCTION);
    lua_pushvalue(l, 1); // duplicate callback, because luaL_ref pops it

    LuaWorker* worker = GetLuaWorkerFromState(l);

    const int old = worker->errorHandler;
    worker->errorHandler = luaL_ref(l, LUA_REGISTRYINDEX);

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

#if defined(KONSTRUKT_PROFILER_ENABLED)
static int GetLuaMemoryInBytes()
{
    assert(g_LuaState);
    return lua_gc(g_LuaState, LUA_GCCOUNT, 0)*1024 +
           lua_gc(g_LuaState, LUA_GCCOUNTB, 0);
}
#endif

static void UpdateLua( void* worker_ )
{
    LuaWorker* worker = (LuaWorker*)worker_;

    {
        ProfileScope("Lua parallel phase");
        // TODO FireLuaEvent(worker->state, g_LuaParallelEvent, 0, false);
    }

    {
        ProfileScope("Lua GC");
        lua_gc(worker->state, LUA_GCCOLLECT, 0);
        SetCounter(MemoryCounter, GetLuaMemoryInBytes());
    }
}

void BeginLuaUpdate()
{
    assert(InSerialPhase());
    LuaWorker* workers = LuaWorkers.data;
    REPEAT(LuaWorkers.length, i)
    {
        assert(workers[i].job == INVALID_JOB_ID);
        workers[i].job = CreateJob({workers[i].jobName, UpdateLua, NULL, &workers[i]});
    }
}

void CompleteLuaUpdate()
{
    assert(InSerialPhase());

    LuaWorker* workers = LuaWorkers.data;

    REPEAT(LuaWorkers.length, i)
    {
        assert(workers[i].job != INVALID_JOB_ID);
        WaitForJobs(&workers[i].job, 1);
        workers[i].job = INVALID_JOB_ID;
    }

    ProfileScope("Lua serial phase");
    // TODO
    //REPEAT(LuaWorkers.length, i)
    //    FireLuaEvent(g_LuaState, g_LuaSerialEvent, 0, false);
}


// --- LuaWorker ---

static void SetLuaModule( lua_State* l, const char* name, lua_CFunction openf )
{
    lua_pushcfunction(l, openf);
    lua_pushstring(l, name);  // argument to open function
    lua_call(l, 1, 1);  // call openf function (module will be at -1)
    lua_setfield(l, -2, name); // set k[name] = module (k is at -2)
}

static void PushEngineTable( lua_State* l )
{
    lua_createtable(l, 0, LuaFunctions.length);

    LuaFunctionDescription* functions = LuaFunctions.data;
    REPEAT(LuaFunctions.length, i)
    {
        lua_pushstring(l, functions[i].name);

        lua_createtable(l, 0, 3);

        lua_pushcfunction(l, functions[i].function);
        lua_setfield(l, -2, "fn");

        lua_pushboolean(l, functions[i].flags & LUA_THREAD_SAFE);
        lua_setfield(l, -2, "thread_safe");

        lua_pushboolean(l, functions[i].flags & LUA_NO_RESULTS);
        lua_setfield(l, -2, "no_results");

        // -3: engine table
        // -2: function name
        // -1: function table

        lua_settable(l, -3);
    }
}

static int PushBootstrapArguments( lua_State* l )
{
    lua_createtable(l, 0, 2);
    SetLuaModule(l, "cjson", luaopen_cjson);
    SetLuaModule(l, "taggedcoro", luaopen_taggedcoro);

    PushEngineTable(l);

    return 2;
}

static void InitLuaWorker( LuaWorker* worker, int index )
{
    worker->state = luaL_newstate();
    worker->errorHandler = LUA_NOREF;
    worker->job = INVALID_JOB_ID;
    FormatBuffer(worker->jobName, MAX_JOB_NAME_SIZE, "update lua worker %d", index);

    lua_State* l = worker->state;

    lua_gc(l, LUA_GCSTOP, 0); // only collect manually

    lua_pushlightuserdata(l, worker);
    lua_setfield(l, LUA_REGISTRYINDEX, LUA_WORKER_KEY);

    // Load standard modules (except for the io, os and package modules)
    luaopen_base(l);
    luaL_requiref(l, LUA_COLIBNAME,   luaopen_coroutine,  true);
    luaL_requiref(l, LUA_TABLIBNAME,  luaopen_table,      true);
    luaL_requiref(l, LUA_STRLIBNAME,  luaopen_string,     true);
    luaL_requiref(l, LUA_BITLIBNAME,  luaopen_bit32,      true);
    luaL_requiref(l, LUA_MATHLIBNAME, luaopen_math,       true);
    luaL_requiref(l, LUA_DBLIBNAME,   luaopen_debug,      true);

    // Register user data types:
    LuaTypeDescription* types = LuaTypes.data;
    REPEAT(LuaTypes.length, i)
    {
        luaL_newmetatable(l, types[i].name);

        if(types[i].gcCallback)
        {
            // set __gc callback
            lua_pushcfunction(l, types[i].gcCallback);
            lua_setfield(l, -2, "__gc");
        }

        // pop metatable
        lua_pop(l, 1);
    }

    PushLuaScript(l, "core/bootstrap/init.lua");
    const int argumentCount = PushBootstrapArguments(l);
    CallLuaFunction(l, argumentCount, 0);

    assert(worker->errorHandler != LUA_NOREF);
}

static void DestroyLuaWorker( LuaWorker* worker )
{
    assert(worker->state);

    lua_State* l = worker->state;

    luaL_unref(l, LUA_REGISTRYINDEX, worker->errorHandler);

    //FireLuaEvent(l, g_LuaShutdownEvent, 0, false);

    lua_close(l);
}

void SetLuaWorkerCount( int count )
{
    assert(InSerialPhase());
    assert(count >= 0);
    assert(count < 1024); // sanity check
    if(LuaWorkers.length < count) // add workers
    {
        const int createdWorkers = count - LuaWorkers.length;
        LuaWorker* workers = AllocateAtEndOfArray(&LuaWorkers, createdWorkers);
        REPEAT(createdWorkers, i)
            InitLuaWorker(&workers[i], i);
    }
    else if(LuaWorkers.length > count) // remove workers
    {
        const int removedWorkers = LuaWorkers.length - count;
        for(int i = count; i < LuaWorkers.length; i++)
            DestroyLuaWorker(GetArrayElement(&LuaWorkers, i));
        PopFromArray(&LuaWorkers, removedWorkers);
    }
}

static LuaWorker* GetLuaWorkerFromState( lua_State* l )
{
    lua_getfield(l, LUA_REGISTRYINDEX, LUA_WORKER_KEY);
    LuaWorker* worker = (LuaWorker*)lua_touserdata(l, -1);
    lua_pop(l, 1);
    assert(worker);
    return worker;
}


// --- loading ---

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

static void PushLuaScript( lua_State* l, const char* vfsPath )
{
    VfsFile* file = OpenVfsFile(vfsPath, VFS_OPEN_READ);
    LoadState state;
    state.file = file;
    const char* source = Format("@%s", vfsPath);
    if(lua_load(l, ReadLuaChunk, &state, source, "t") != LUA_OK)
    {
        FatalError("%s", lua_tostring(l, -1));
        lua_pop(l, 2); // pop error function and error message
    }
    CloseVfsFile(file);
}


// --- calling ---

static void HandleLuaCallResult( lua_State* l, int result )
{
    switch(result)
    {
        case LUA_OK:
            return;

        case LUA_ERRRUN:
            {
                const char* message = lua_tostring(l, -1);
                FatalError("%s", message);
                lua_pop(l, 1);
            }

        case LUA_ERRMEM:
            FatalError("Lua encountered an memory allocation error.");

        case LUA_ERRERR:
            FatalError("Lua encountered an error while executing the error handler.");

        default:
            FatalError("Unknown call result.");
    }
}

static int Lua_ErrorProxy( lua_State* l )
{
    LuaWorker* worker = GetLuaWorkerFromState(l);
    if(worker->errorHandler != LUA_NOREF)
    {
        lua_rawgeti(l, LUA_REGISTRYINDEX, worker->errorHandler);
        lua_pushvalue(l, 1); // push message to the top
        lua_call(l, 1, 1);
    }
    else
    {
        const char* message = lua_tostring(l, 1);
        lua_pushfstring(l, "(Via fallback error handler:) %s", message);
    }
    return 1;
}

static int CallLuaFunction( lua_State* l, int argumentCount, int expectedReturnValueCount )
{
    assert(argumentCount >= 0);
    assert(expectedReturnValueCount >= 0 ||
           expectedReturnValueCount == LUA_MULTRET);

    LuaWorker* worker = GetLuaWorkerFromState(l);

    lua_pushcfunction(l, Lua_ErrorProxy);
    lua_insert(l, -(argumentCount+2)); // Move before function and arguments)

    // The stack should now look like this:
    // error proxy
    // function
    // arg 1
    // ...
    // arg n

    const int stackSizeBeforeCall = lua_gettop(l);
    const int callResult = lua_pcall(
        l, argumentCount,
        expectedReturnValueCount,
        -(argumentCount+2) // error func
    );
    const int stackSizeAfterCall = lua_gettop(l);

    const int poppedValueCount = argumentCount + 1; // arguments and function
    const int returnValueCount = stackSizeAfterCall -
                                 (stackSizeBeforeCall-poppedValueCount);
    lua_remove(l, -(returnValueCount+1)); // Remove error function

    HandleLuaCallResult(l, callResult);

    return returnValueCount;
}


// --- Utils ---

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

void PushIdToLua( lua_State* l, unsigned int id )
{
    lua_pushunsigned(l, id);
}

unsigned int GetIdFromLua( lua_State* l, int stackPosition )
{
    return lua_tounsigned(l, stackPosition);
}

unsigned int CheckIdFromLua( lua_State* l, int stackPosition )
{
    return luaL_checkunsigned(l, stackPosition);
}


// --- LuaEvent ---

int RegisterLuaEvent( const char* name )
{
    return 0;
}

int FireLuaEvent( lua_State* l, int id, int argumentCount, bool pushReturnValues )
{
    return 0;
}


