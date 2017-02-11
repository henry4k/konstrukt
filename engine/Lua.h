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

enum LuaArrayType
{
    LUA_BOOL_ARRAY,
    LUA_DOUBLE_ARRAY,
    LUA_LONG_ARRAY
};


void InitLua();
void DestroyLua();

lua_State* GetLuaState();

void UpdateLua();

/**
 * Registers a native function in Lua.
 */
void RegisterFunctionInLua( const char* name, lua_CFunction fn );

/**
 * Registers a user data type by name.
 *
 * User data created by #PushUserDataToLua is annotated with type information,
 * so #GetUserDataFromLua can assure that the element has the correct type.
 *
 * @param gcCallback
 * Function that is called before Lua frees garbage collected user data.
 * May be `NULL` - in this case Lua doesn't notify you at all.
 */
void RegisterUserDataTypeInLua( const char* name, lua_CFunction gcCallback );

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

/**
 * Loads a script from `vfsPath` and executes it.
 */
void RunLuaScript( lua_State* l, const char* vfsPath );

/**
 * Registers a new event and returns its id.
 *
 * @note
 * An event may only be registered once.
 *
 * @return
 * The events id.
 */
int RegisterLuaEvent( const char* name );

/**
 * Runs the script callback associated with the given event.
 *
 * @note
 * If no callback was set for the event,
 * it will behave as if an empty function was called.
 *
 * @param id
 * Event id returned by #RegisterLuaEvent.
 *
 * @param argumentCount
 * Number of elements in the lua stack, that shall be used
 * as function parameters for the callback.
 *
 * @param pushReturnValues
 * Whether to push values returned by the callback in the stack.
 *
 * @return
 * Number of values that were pushed in the stack or 0.
 */
int FireLuaEvent( lua_State* l, int id, int argumentCount, bool pushReturnValues );

#endif
