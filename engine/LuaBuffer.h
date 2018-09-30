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
 * If 0 the string length will be computed automatically - assuming it is a
 * zero terminated string.
 */
void AddStringToLuaBuffer(LuaBuffer* buffer, const char* string, int length);

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
