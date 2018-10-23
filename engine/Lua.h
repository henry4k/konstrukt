#ifndef __KONSTRUKT_LUA__
#define __KONSTRUKT_LUA__

extern "C"
{
#define LUA_LIB
#include <lua.h>
#include <lauxlib.h>
}


enum
{
    INVALID_LUA_EVENT = -1
};

struct LuaWorker;
struct LuaBuffer;

struct LuaEventListener
{
    // Internal properties - do not use!
    LuaWorker* worker;
    const char* name;
};


// --- General ---

void InitLua();
void DestroyLua();

/**
 * Starts a job for each Lua worker in which they run their parallel/work
 * phase.
 */
void BeginLuaUpdate();

/**
 * Await completion of all Lua worker jobs and run their
 * synchronous/synchronization phases.
 */
void CompleteLuaUpdate();

/**
 * Registers a native function in Lua.
 */
void RegisterLuaFunction( const char* name, lua_CFunction fn );
#define REGISTER_LUA_FUNCTION(Name) RegisterLuaFunction(#Name, Lua_##Name)
#define RegisterFunctionInLua RegisterLuaFunction


/**
 * Registers a user data type by name.
 *
 * User data created by #PushUserDataToLua is annotated with type information,
 * so #GetUserDataFromLua can assure that the element has the correct type.
 *
 * @param gcCallback
 * Function that is called before Lua frees garbage collected user data.
 * May be `NULL` - in this case Lua doesn't notify you at all.
 *
 * TODO: Because gcCallbacks are now executed in the LuaWorkers thread
 * they'd need to be thread-safe functions.  Maybe its better to decide these
 * things in Lua directly as engine functions have thread-safety annotations
 * now.
 */
void RegisterLuaType( const char* name, lua_CFunction gcCallback );
#define RegisterUserDataTypeInLua RegisterLuaType


// --- LuaWorker ---

/**
 * Creates or destroys workers so there are exactly `count` living ones.
 */
void SetLuaWorkerCount( int count );


// --- Utils ---

/**
 * Allocates memory for a user defined data structure in Lua
 * and pushes it on top of the stack.
 *
 * @param size
 * Data size in bytes.
 *
 * Since user data of the same type can still have different sizes,
 * it's your responsibility to ensure correct data access.
 *
 * @return
 * The allocated, but uninitialized, user data
 */
void* PushUserDataToLua( lua_State* l, const char* typeName, int size );

/**
 * Copies a user data to Lua and pushes it on top of the stack.
 *
 * @param size
 * Data size in bytes.
 *
 * Since user data of the same type can still have different sizes,
 * it's your responsibility to ensure correct data access.
 *
 * @param data
 * Pointer to the data, that is being copied.
 */
void CopyUserDataToLua( lua_State* l, const char* typeName, int size, const void* data );

/**
 * Checks if the element at `stackPosition` has the requested user data type
 * and retrieves it.
 *
 * @return
 * Pointer to the user data or `NULL` if the type does not match.
 *
 * @note
 * Alternatively #CheckUserDataFromLua can be used to retrieve and check userdata.
 * It raises an Lua exception if the userdata doesn't match the given type.
 */
void* GetUserDataFromLua( lua_State* l, int stackPosition, const char* typeName );

/**
 * Like #GetUserDataFromLua, but may raise an Lua error.
 *
 * @return
 * Pointer to the user data.
 */
void* CheckUserDataFromLua( lua_State* l, int stackPosition, const char* typeName );

/**
 * Pushes a pointer (light userdata) on top of the stack.
 *
 * A `NULL` pointer will be pushed as `nil` value.
 *
 * @note
 * Since light userdata has no meta table, it doesn't provide a gc callpack or
 * a possibillity to check its actual C types. The only (!) advantage is, that
 * its are equal to each other light userdata with the same pointer.
 */
void PushPointerToLua( lua_State* l, void* pointer );

/**
 * Retrieves the user pointer (light userdata) at the given `stackPosition`
 * and returns it.
 *
 * @return
 * Pointer or `NULL` if an error occures: e.g. `stackPosition` is invalid or it
 * doesn't hold a light userdata object.
 *
 * @note
 * Alternatively #CheckPointerFromLua can be used to retrieve and check
 * light userdata. It raises an Lua exception if the userdata doesn't match the
 * given type.
 */
void* GetPointerFromLua( lua_State* l, int stackPosition );

/**
 * Like #GetPointerFromLua, but may raise an Lua error.
 */
void* CheckPointerFromLua( lua_State* l, int stackPosition );

void PushIdToLua( lua_State* l, unsigned int id );
unsigned int GetIdFromLua( lua_State* l, int stackPosition );
unsigned int CheckIdFromLua( lua_State* l, int stackPosition );


// --- Events ---

/**
 * Use this constant to initialize unused event listeners.
 */
static const LuaEventListener INVALID_LUA_EVENT_LISTENER = {NULL, NULL};

inline bool IsValidLuaEventListener( LuaEventListener listener )
{
    return listener.worker && listener.name;
}

LuaEventListener GetLuaEventListener( lua_State* l, const char* name );

/**
 * Creates a new event for Lua.
 *
 * May block if other threads are currently creating events.
 *
 * @return A buffer which takes event parameters as values.
 * It may only be used to add new values.
 *
 * @see CompleteLuaEvent
 */
LuaBuffer* BeginLuaEvent( LuaEventListener listener );

/**
 * After all parameters were added this is used to complete the event.
 */
void CompleteLuaEvent( LuaEventListener listener );

#endif
