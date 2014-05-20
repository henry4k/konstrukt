#include <string.h>
#include <vector>

#include "Common.h"
#include "Lua.h"

extern "C"
{
#define LUA_LIB
#include <lualib.h>
#include <lua_cjson.h>
}


lua_State* g_LuaState = NULL;

const int MAX_LUA_EVENT_NAME_LENGTH = 32;
struct LuaEvent
{
    char name[MAX_LUA_EVENT_NAME_LENGTH];
    int callbackReference;
};
std::vector<LuaEvent> g_LuaEvents;


int Lua_SetEventCallback( lua_State* l );
int Lua_DefaultErrorFunction( lua_State* l );
int Lua_Log( lua_State* l );


bool InitLua()
{
    assert(g_LuaState == NULL);
    assert(g_LuaEvents.empty());

    Log("Using " LUA_COPYRIGHT);

    g_LuaState = luaL_newstate();
    g_LuaEvents.clear();

    lua_gc(g_LuaState, LUA_GCSTOP, 0); // only collect manually
    luaL_openlibs(g_LuaState);

    luaopen_cjson(g_LuaState);
    lua_setglobal(g_LuaState, "cjson");

    lua_createtable(g_LuaState, 0, 0);
    lua_setglobal(g_LuaState, "NATIVE");

    RegisterFunctionInLua("SetEventCallback", Lua_SetEventCallback);
    RegisterFunctionInLua("DefaultErrorFunction", Lua_DefaultErrorFunction);
    RegisterFunctionInLua("Log", Lua_Log);

    return true;
}

void DestroyLua()
{
    assert(g_LuaState);

    lua_close(g_LuaState);
    g_LuaState = NULL;

    g_LuaEvents.clear();
}

lua_State* GetLuaState()
{
    return g_LuaState;
}

int GetLuaMemoryInBytes()
{
    assert(g_LuaState);
    return lua_gc(g_LuaState, LUA_GCCOUNT, 0)*1024 +
           lua_gc(g_LuaState, LUA_GCCOUNTB, 0);
}

void UpdateLua()
{
    const int memBeforeGC = GetLuaMemoryInBytes();
    lua_gc(g_LuaState, LUA_GCCOLLECT, 0);
    const int memAfterGC = GetLuaMemoryInBytes();

    Log("LUA GC UPDATE: %d bytes in use. %d bytes collected.",
        memAfterGC,
        memBeforeGC-memAfterGC
    );
}

bool RegisterFunctionInLua( const char* name, lua_CFunction fn )
{
    lua_getglobal(g_LuaState, "NATIVE");
    lua_pushcfunction(g_LuaState, fn);
    lua_setfield(g_LuaState, -2, name);
    lua_pop(g_LuaState, 1);
    Log("Registered lua function: %s", name);
    return true;
}

bool RegisterUserDataTypeInLua( const char* name, lua_CFunction gcCallback )
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

    Log("Registered lua type: %s", name);
    return true;
}

void* PushUserDataToLua( lua_State* l, const char* typeName, int size )
{
    void* data = lua_newuserdata(l, size);

    luaL_getmetatable(l, typeName);
    if(!lua_istable(l, -1))
    {
        lua_pop(l, 2);
        return NULL;
    }
    lua_setmetatable(l, -2);

    return data;
}

bool CopyUserDataToLua( lua_State* l, const char* typeName, int size, const void* data )
{
    void* luaData = PushUserDataToLua(l, typeName, size);
    if(!luaData)
        return false;
    memcpy(luaData, data, size);
    return true;
}

void* GetUserDataFromLua( lua_State* l, int stackPosition, const char* typeName )
{
    void* data = lua_touserdata(l, stackPosition);
    if(!data)
    {
        Error("not userdata");
        return NULL;
    }

    const bool dataHasMetatable = lua_getmetatable(l, stackPosition);
    if(!dataHasMetatable)
    {
        Error("no metatable attached");
        return NULL;
    }

    luaL_getmetatable(l, typeName);
    if(!lua_istable(l, -1) || !lua_rawequal(l, -2, -1))
    {
        // pop userdatas metatable and the types metatable
        lua_pop(l, 2);
        Error("type unknown");
        return NULL;
    }

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

void PushLuaErrorFunction( lua_State* l )
{
    // TODO: Let the script change the error function.
    lua_pushcfunction(l, Lua_DefaultErrorFunction);
}

int Lua_DefaultErrorFunction( lua_State* l )
{
    //lua_pushstring(l, "DEFAULT ERROR FUNCTION CALLED! TODO TODO TODO!");
    lua_pushvalue(l, 1);
    return 1;
}

int FindLuaEventByName( const char* name )
{
    assert(name != NULL);
    for(int i = 0; i < g_LuaEvents.size(); i++)
        if(strncmp(name, g_LuaEvents[i].name, MAX_LUA_EVENT_NAME_LENGTH) == 0)
            return i;
    return -1;
}

int RegisterLuaEvent( const char* name )
{
    assert(FindLuaEventByName(name) == -1);
    assert(strlen(name) < MAX_LUA_EVENT_NAME_LENGTH);

    LuaEvent event;
    memset(&event, 0, sizeof(event));
    strncpy(event.name, name, MAX_LUA_EVENT_NAME_LENGTH);
    event.callbackReference = LUA_NOREF;

    g_LuaEvents.push_back(event);

    Log("Registered lua event: %s", name);

    const int id = g_LuaEvents.size()-1;
    if(id >= 0 || id < g_LuaEvents.size())
        return id;
    else
        return LUA_INVALID_EVENT;
}

int FireLuaEvent( lua_State* l, int id, int argumentCount, bool pushReturnValues )
{
    assert(id >= 0 && id < g_LuaEvents.size());
    assert(argumentCount >= 0);

    LuaEvent* event = &g_LuaEvents[id];

    if((event->callbackReference == LUA_NOREF) ||
       (event->callbackReference == LUA_REFNIL))
    {
        lua_pop(l, argumentCount);
        return 0;
    }

    PushLuaErrorFunction(l);
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
    const int callResult = lua_pcall(
        l, argumentCount,
        pushReturnValues ? LUA_MULTRET : 0, // return value count
        -(argumentCount+2) // error func
    );
    const int stackSizeAfterCall = lua_gettop(l);

    const int poppedValueCount = argumentCount + 1;
    const int returnValueCount =
        stackSizeAfterCall - (stackSizeBeforeCall-poppedValueCount);
    lua_remove(l, -(returnValueCount+1)); // Remove error function

    switch(callResult)
    {
        case LUA_ERRRUN:
            {
                const char* message = lua_tostring(l, -1);
                Error("%s", message);
                lua_pop(l, 1);
            }
            break;

        case LUA_ERRMEM:
            FatalError("Lua encountered a memory allocation error.");

        case LUA_ERRERR:
            FatalError("Lua encountered an error while executing the error function.");

        case 0:
            break;

        default:
            FatalError("Unknown call result.");
    }

    return returnValueCount;
}

bool PushArrayToLua( lua_State* l, LuaArrayType elementType, int elementCount, const void* elements )
{
    lua_createtable(l, elementCount, 0);

    switch(elementType)
    {
#define HANDLE_TYPE(TypeEnum, Typedef, PushFn) \
        case TypeEnum: \
            for(int i = 0; i < elementCount; i++) \
            { \
                PushFn(l, ((const Typedef*)elements)[i]); \
                lua_rawseti(l, -2, i+1); \
            } \
            break;

        HANDLE_TYPE(LUA_BOOL_ARRAY, bool, lua_pushboolean)
        HANDLE_TYPE(LUA_DOUBLE_ARRAY, double, lua_pushnumber)
        HANDLE_TYPE(LUA_LONG_ARRAY, long, lua_pushinteger)
#undef HANDLE_TYPE

        default:
            FatalError("Unsupported element type.");
    }

    return true;
}

bool GetArrayFromLua( lua_State* l, int stackPosition, LuaArrayType elementType, int maxElementCount, void* destination )
{
    if(lua_type(l, stackPosition) != LUA_TTABLE)
        return false;

    const int totalArraySize = GetLuaArraySize(l, stackPosition);
    const int elementCount =
        (totalArraySize < maxElementCount) ? totalArraySize : maxElementCount;

    switch(elementType)
    {
#define HANDLE_TYPE(TypeEnum, Typedef, GetFn) \
        case TypeEnum: \
            for(int i = 0; i < elementCount; i++) \
            { \
                lua_rawgeti(l, stackPosition, i+1); \
                ((Typedef*)destination)[i] = GetFn(l, -1); \
            } \
            break;

        HANDLE_TYPE(LUA_BOOL_ARRAY, bool, lua_toboolean)
        HANDLE_TYPE(LUA_DOUBLE_ARRAY, double, lua_tonumber)
        HANDLE_TYPE(LUA_LONG_ARRAY, long, lua_tointeger)
#undef HANDLE_TYPE

        default:
            FatalError("Unsupported element type.");
    }

    return true;
}

int GetLuaArraySize( lua_State* l, int stackPosition )
{
    if(lua_type(l, stackPosition) == LUA_TTABLE)
        return lua_rawlen(l, stackPosition);
    else
        return 0;
}

int Lua_SetEventCallback( lua_State* l )
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

int Lua_Log( lua_State* l )
{
    const char* message = luaL_checkstring(l, 1);
    Log("%s", message);
    return 0;
}
