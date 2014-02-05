#include <vector>

#include "Common.h"
#include "Debug.h"
#include "Config.h"
#include "Squirrel.h"

#include <sqstdio.h>
#include <sqstdmath.h>
#include <sqstdstring.h>
#include <sqstdsystem.h>
#include <sqstdblob.h>
#include <sqstdaux.h>

void Squirrel_print( HSQUIRRELVM vm, const char* format, ... );
void Squirrel_error( HSQUIRRELVM vm, const char* format, ... );
SQInteger Squirrel_filename( HSQUIRRELVM vm );

struct StaticSquirrelFn
{
    const char* name;
    SQFUNCTION func;
    SQInteger argCount;
    const char* typeMask;
};

std::vector<StaticSquirrelFn>& StaticSquirrelFunctions()
{
    static std::vector<StaticSquirrelFn> r;
    return r;
}

HSQUIRRELVM g_SquirrelVM;
HSQOBJECT g_NativeTable;
HSQOBJECT g_SquirrelCallbacks[SQCALLBACK_COUNT];

bool InitSquirrel()
{
    Log("Using %s\n%s", SQUIRREL_VERSION, SQUIRREL_COPYRIGHT);

    const HSQUIRRELVM vm = sq_open(1024);
    g_SquirrelVM = vm;

    sq_setprintfunc(vm, Squirrel_print, Squirrel_error);

    if(IsDebugging(DEBUG_SQUIRREL))
    {
        sq_enabledebuginfo(vm, true);
    }

    sq_pushroottable(vm);
    sq_pushstring(vm, "native", -1);
    sq_newtable(vm);
    sq_getstackobj(vm, -1, &g_NativeTable);
    sq_addref(vm, &g_NativeTable);

    sq_pushstring(vm, "io", -1);
    sq_newtable(vm);
    sqstd_register_iolib(vm);
    sq_newslot(vm, -3, false);

    sq_pushstring(vm, "blob", -1);
    sq_newtable(vm);
    sqstd_register_bloblib(vm);
    sq_newslot(vm, -3, false);

    sq_pushstring(vm, "string", -1);
    sq_newtable(vm);
    sqstd_register_stringlib(vm);
    sq_newslot(vm, -3, false);

    sq_pushstring(vm, "math", -1);
    sq_newtable(vm);
    sqstd_register_mathlib(vm);
    sq_newslot(vm, -3, false);

    sq_pushstring(vm, "system", -1);
    sq_newtable(vm);
    sqstd_register_systemlib(vm);
    sq_newslot(vm, -3, false);

    sq_newslot(vm, -3, false); // internal table

    sqstd_seterrorhandlers(vm);

    for(int i = 0; i < StaticSquirrelFunctions().size(); ++i)
    {
        StaticSquirrelFn& fn = StaticSquirrelFunctions()[i];
        RegisterFunctionInSquirrel(
            fn.name,
            fn.func,
            fn.argCount,
            fn.typeMask
        );
    }
    StaticSquirrelFunctions().clear();

    for(int i = 0; i < SQCALLBACK_COUNT; ++i)
        sq_resetobject(&g_SquirrelCallbacks[i]);
    //memset(g_SquirrelCallbacks, 0, sizeof(g_SquirrelCallbacks));

    return true;
}

void DestroySquirrel()
{
    for(int i = 0; i < SQCALLBACK_COUNT; ++i)
        sq_release(g_SquirrelVM, &g_SquirrelCallbacks[i]);

    sq_release(g_SquirrelVM, &g_NativeTable);

    sq_close(g_SquirrelVM);
}

void UpdateSquirrel( float timeDelta )
{
    const int deletedCycles = sq_collectgarbage(g_SquirrelVM);
    if(deletedCycles && IsDebugging(DEBUG_SQUIRREL))
    {
        Log("Deleted %d reference cycles.", deletedCycles);
    }
}

HSQUIRRELVM GetSquirrelVM()
{
    return g_SquirrelVM;
}

void RegisterFunctionInSquirrel( const char* name, SQFUNCTION func, SQInteger argCount, const char* typeMask )
{
    const HSQUIRRELVM vm = g_SquirrelVM;

    sq_pushobject(vm, g_NativeTable);
    sq_pushstring(vm, name, -1);
    sq_newclosure(vm, func, 0);
    sq_setparamscheck(vm, argCount, typeMask);
    sq_setnativeclosurename(vm, -1, name);
    sq_newslot(vm, -3, SQFalse);
    sq_pop(vm, 1);

    const char* vargString = argCount<0 ? " ..." : "";
    Log("Registered squirrel function: %s(%s%s)", name, typeMask, vargString);
}

bool RegisterStaticFunctionInSquirrel_( const char* name, SQFUNCTION func, SQInteger argCount, const char* typeMask )
{
    StaticSquirrelFn fn;
    fn.name = name;
    fn.func = func;
    fn.argCount = argCount;
    fn.typeMask = typeMask;
    StaticSquirrelFunctions().push_back(fn);
    return true;
}

bool RunSquirrelFile( const char* file )
{
    Log("Running %s ...", file);
    return SQ_SUCCEEDED(sqstd_dofile(g_SquirrelVM, file, false, true));
}

void PushIntArrayToSquirrel( HSQUIRRELVM vm, const int* values, int length )
{
    sq_newarray(vm, length);
    for(int i = 0; i < length; ++i)
    {
        sq_pushinteger(vm, values[i]);
        sq_arrayinsert(vm, -2, i);
    }
}

void PushFloatArrayToSquirrel( HSQUIRRELVM vm, const float* values, int length )
{
    sq_newarray(vm, length);
    for(int i = 0; i < length; ++i)
    {
        sq_pushfloat(vm, values[i]);
        sq_arrayinsert(vm, -2, i);
    }
}

void Squirrel_print( HSQUIRRELVM vm, const char* format, ... )
{
    va_list vl;
    va_start(vl, format);
    LogV(LOG_INFO, format, vl);
    va_end(vl);
}

void Squirrel_error( HSQUIRRELVM vm, const char* format, ... )
{
    va_list vl;
    va_start(vl, format);
    LogV(LOG_ERROR, format, vl);
    va_end(vl);
}

void* CreateUserDataInSquirrel( HSQUIRRELVM vm, int size, SQRELEASEHOOK releaseHook )
{
    assert(size > 0);
    void* userData = sq_newuserdata(vm, size);
    if(releaseHook)
        sq_setreleasehook(vm, -1, releaseHook);
    return userData;
}

void* PushUserDataToSquirrel( HSQUIRRELVM vm, const void* source, int size, SQRELEASEHOOK releaseHook )
{
    void* userData = CreateUserDataInSquirrel(vm, size, releaseHook);
    memcpy(userData, source, size);
    return userData;
}

void SetSquirrelCallback( SquirrelCallback callback, HSQUIRRELVM vm, int stackIndex )
{
    sq_release(g_SquirrelVM, &g_SquirrelCallbacks[callback]);
    sq_getstackobj(vm, stackIndex, &g_SquirrelCallbacks[callback]);
    sq_addref(vm, &g_SquirrelCallbacks[callback]);
}

void ClearSquirrelCallback( SquirrelCallback callback )
{
    sq_release(g_SquirrelVM, &g_SquirrelCallbacks[callback]);
}

bool FireSquirrelCallback( SquirrelCallback callback, int argumentCount, bool pushReturnValue )
{
    HSQUIRRELVM vm = g_SquirrelVM;

    sq_pushobject(vm, g_SquirrelCallbacks[callback]);
    sq_pushroottable(vm);
    for(int i = argumentCount; i > 0; --i)
    {
        sq_push(vm, -(argumentCount+2));
    }

    const bool success = SQ_SUCCEEDED( sq_call(vm, argumentCount+1, pushReturnValue, true) );
    // TODO: Has sq_call been handled correctly?
    if(pushReturnValue && success)
    {
        HSQOBJECT tmp;
        sq_getstackobj(vm, -1, &tmp);
        sq_addref(vm, &tmp);

        sq_pop(vm, 2); // Pop closure and return value

        sq_pushobject(vm, tmp);
        sq_release(vm, &tmp);
    }
    else
    {
        sq_pop(vm, 1); // Pop closure
    }

    return success;
}
