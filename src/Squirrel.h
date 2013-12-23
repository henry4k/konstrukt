#ifndef __SQUIRREL__
#define __SQUIRREL__

#include <squirrel.h>

// --- Misc ---

bool InitSquirrel();
void DestroySquirrel();
void UpdateSquirrel( float timeDelta );
HSQUIRRELVM GetSquirrelVM();
bool RunSquirrelFile( const char* file );


// --- Native functions ---

void RegisterFunctionInSquirrel( const char* name, SQFUNCTION func, SQInteger argCount, const char* typeMask );

bool RegisterStaticFunctionInSquirrel_( const char* name, SQFUNCTION func, SQInteger argCount, const char* typeMask );
#define RegisterStaticFunctionInSquirrel( Name, ... ) \
    bool Squirrel_##Name##_Registered = RegisterStaticFunctionInSquirrel_(#Name, Squirrel_##Name, __VA_ARGS__)



// --- Userdata ---

void* CreateUserDataInSquirrel( HSQUIRRELVM vm, int size, SQRELEASEHOOK releaseHook );
void* PushUserDataToSquirrel( HSQUIRRELVM vm, const void* source, int size, SQRELEASEHOOK releaseHook );



// --- Callbacks ---

enum SquirrelCallback
{
    SQCALLBACK_KEY_CONTROL,
    SQCALLBACK_AXIS_CONTROL,
    SQCALLBACK_AUDIO_SOURCE_STOP,
    SQCALLBACK_STATIC_TILE_MESH_GENRATOR,
    SQCALLBACK_STATIC_TILE_SOLID_GENRATOR,
    SQCALLBACK_COUNT
};

/**
 *
 *
 */
void SetSquirrelCallback( SquirrelCallback callback, HSQUIRRELVM vm, int stackIndex );
void ClearSquirrelCallback( SquirrelCallback callback );

/**
 * Pops the arguments from the stack and calls the squirrel function.
 * Note that you musn't push an environment object!
 * @return
 *      `false` if a vm exception occured.
 */
bool FireSquirrelCallback( SquirrelCallback callback, int argumentCount, bool pushReturnValue );



#endif
