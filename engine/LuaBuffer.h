#ifndef __KONSTRUKT_LUA_BUFFER__
#define __KONSTRUKT_LUA_BUFFER__

#include "Lua.h"


enum LuaBufferType
{
    /**
     * Stores data in a platform specific binary format.  Suitable for
     * transferring data between local Lua states.
     */
    NATIVE_LUA_BUFFER
};

enum LuaBufferStringFlags
{
    /**
     * Use this if the string pointer is valid at least while any Lua states
     * it is passed to exist.
     */
    LUA_BUFFER_LITERAL_STRING = (1 << 0)
};

/**
 * Stores Lua values in various formats.
 *
 * @see LuaBufferType
 */
struct LuaBuffer;


LuaBuffer* CreateLuaBuffer(LuaBufferType type);
void ReferenceLuaBuffer( LuaBuffer* buffer );
void ReleaseLuaBuffer( LuaBuffer* buffer );

void AddNilToLuaBuffer(LuaBuffer* buffer);
void AddBooleanToLuaBuffer(LuaBuffer* buffer, bool value);
void AddIntegerToLuaBuffer(LuaBuffer* buffer, lua_Integer value);
void AddNumberToLuaBuffer(LuaBuffer* buffer, lua_Number value);

/**
 * @param length
 * If its 0 the string length will be computed automatically - assuming it is
 * a zero terminated string.
 *
 * @param flags
 * See #LuaBufferStringFlags
 */
void AddStringToLuaBuffer(LuaBuffer* buffer,
                          const char* string,
                          int length,
                          int flags);

void BeginListInLuaBuffer(LuaBuffer* buffer);
void EndListInLuaBuffer(LuaBuffer* buffer);

/**
 * Remove all elements from the buffer - this should be a fast action.
 */
void ClearLuaBuffer(LuaBuffer* buffer);

/**
 * @param dataOut
 * Data pointer is placed there.
 *
 * @return
 * Data size
 */
int GetLuaBufferData(const LuaBuffer* buffer, const char** dataOut);

/**
 * @return
 * Amount of new stack entries (values) created.
 */
int PushLuaBufferToLua(LuaBuffer* buffer, lua_State* state);

/**
 * @return
 * Amount of new stack entries (values) created.
 */
int PushLuaBufferDataToLua(LuaBufferType type,
                           const char* data,
                           int length,
                           lua_State* state);

#endif
