#include <assert.h>
#include <string.h>
#include <vector>

#include "Lua.h"

extern "C"
{
#define LUA_LIB
#include <lualib.h>
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


bool InitLua()
{
    g_LuaState = luaL_newstate();
    g_LuaEvents.clear();

    lua_gc(g_LuaState, LUA_GCSTOP, 0); // only collect manually
    luaL_openlibs(g_LuaState);

    RegisterFunctionInLua("SetEventCallback", Lua_SetEventCallback);
    RegisterFunctionInLua("DefaultErrorFunction", Lua_DefaultErrorFunction);

    return true;
}

void FreeLua()
{
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
    return lua_gc(g_LuaState, LUA_GCCOUNT, 0)*1024 +
           lua_gc(g_LuaState, LUA_GCCOUNTB, 0);
}

void UpdateLua()
{
    const int memBeforeGC = GetLuaMemoryInBytes();
    lua_gc(g_LuaState, LUA_GCCOLLECT, 0);
    const int memAfterGC = GetLuaMemoryInBytes();

    printf("LUA GC UPDATE: %d bytes in use. %d bytes collected.\n",
        memAfterGC,
        memBeforeGC-memAfterGC
    );
}

void RegisterFunctionInLua( const char* name, lua_CFunction fn )
{
    lua_pushcfunction(g_LuaState, fn);
    lua_setglobal(g_LuaState, name);
    printf("Registered lua function %s\n", name);
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
    {
        lua_pop(l, 2);
        return NULL;
    }
    lua_setmetatable(l, -2);

    return data;
}

void* GetUserDataFromLua( lua_State* l, int stackPosition, const char* typeName )
{
    void* data = lua_touserdata(l, stackPosition);
    if(!data)
        return NULL;

    const bool dataHasMetatable = lua_getmetatable(l, -1);
    if(!dataHasMetatable)
    {
        // pop userdata
        lua_pop(l, 1);
        return NULL;
    }

    luaL_getmetatable(l, typeName);
    if(!lua_istable(l, -1) || lua_compare(l, -2, -1, LUA_OPEQ))
    {
        // pop userdata, its metatable and nil
        lua_pop(l, 3);
        return NULL;
    }

    // pop both metatables
    lua_pop(l, 2);
    return data;
}

void* CheckUserDataFromLua( lua_State* l, int stackPosition, const char* typeName )
{
    return luaL_checkudata(l, stackPosition, typeName);
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

    const int id = g_LuaEvents.size()-1;
    assert(id >= 0 && id < g_LuaEvents.size());
    return id;
}

int FireLuaEvent( lua_State* l, int id, int argumentCount, bool pushReturnValues )
{
    assert(id >= 0 && id < g_LuaEvents.size());
    assert(argumentCount >= 0);

    LuaEvent* event = &g_LuaEvents[id];

    if(event->callbackReference == LUA_NOREF)
    {
        lua_pop(l, argumentCount);
        return 0;
    }

    PushLuaErrorFunction(l);

    lua_rawgeti(l, LUA_REGISTRYINDEX, event->callbackReference);

    // Move arguments to the top, i.e. behind error func and callback:
    for(int i = 0; i < argumentCount; i++)
        lua_insert(l, -(argumentCount+2));

    const int stackSizeBeforeCall = lua_gettop(l);
    const int callResult = lua_pcall(
        l, argumentCount,
        pushReturnValues ? LUA_MULTRET : 0, // return value count
        -(argumentCount+2) // error func
    );
    const int stackSizeAfterCall = lua_gettop(l);

    switch(callResult)
    {
        case LUA_ERRRUN:
            {
                const char* message = lua_tostring(l, -1);
                printf("Error: %s\n", message);
                lua_pop(l, 1);
            }
            break;

        case LUA_ERRMEM:
            {
                printf("Lua encountered a memory allocation error.\n");
                assert(false);
            }
            break;

        case LUA_ERRERR:
            printf("Lua encountered an error while executing the error function.\n");
            assert(false);
            break;

        case 0:
            break;

        default:
            assert(!"Unknown call result.");
    }

    const int returnValueCount = stackSizeAfterCall-stackSizeBeforeCall;
    return returnValueCount;
}

int Lua_SetEventCallback( lua_State* l )
{
    const char* name = luaL_checkstring(l, 1);
    const int id = FindLuaEventByName(name);
    if(id < 0)
        return luaL_error(l, "No event called '%s' exists.", name);

    lua_pushvalue(l, 2);
    g_LuaEvents[id].callbackReference = luaL_ref(l, LUA_REGISTRYINDEX);
    return 0;
}
