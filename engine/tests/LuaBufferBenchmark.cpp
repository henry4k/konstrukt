#include <stdlib.h> // srand, rand, RAND_MAX
#include <time.h> // time

#include "../Lua.h"
#include "../LuaBuffer.h"
#include "../Config.h"
#include "../Common.h"
#include "TestTools.h"


int OuterLoopCount;
int InnerLoopCount;
int ValueCount;
int ListStartChance;
int ListEndChance;


static int Lua_Test( lua_State* l )
{
    Require(lua_gettop(l) == 1);
    return 0;
}

static void PushValue(lua_State* l, LuaBuffer* buffer, int* pushedValues)
{
    if(*pushedValues >= ValueCount)
        return;

    (*pushedValues)++;

    if(rand() < ListStartChance)
    {
        BeginListInLuaBuffer(buffer);
        while(*pushedValues < ValueCount &&
              rand() >= ListEndChance)
        {
            PushValue(l, buffer, pushedValues);
        }
        EndListInLuaBuffer(buffer);
    }
    else
    {
        AddIntegerToLuaBuffer(buffer, *pushedValues);
    }
}

InlineTest("Basic Test")
{
    REPEAT(OuterLoopCount, i)
    {
        lua_State* l = luaL_newstate();
        lua_gc(l, LUA_GCSTOP, 0); // only collect manually

        LuaBuffer* buffer = CreateLuaBuffer(NATIVE_LUA_BUFFER);
        ReferenceLuaBuffer(buffer);

        REPEAT(InnerLoopCount, j)
        {
            int pushedValues = 0;

            BeginListInLuaBuffer(buffer);
            while(pushedValues < ValueCount)
                PushValue(l, buffer, &pushedValues);
            EndListInLuaBuffer(buffer);

            lua_pushcfunction(l, Lua_Test);
            const int args = PushLuaBufferToLua(buffer, l);
            lua_call(l, args, 0);

            ClearLuaBuffer(buffer);
        }

        ReleaseLuaBuffer(buffer);
        lua_close(l);
    }
}

int main( int argc, char** argv )
{
    InitTests(argc, argv);
    OuterLoopCount = GetConfigInt("test.outer-loop-count", 1);
    InnerLoopCount = GetConfigInt("test.inner-loop-count", 1);
    ValueCount = GetConfigInt("test.value-count", 1);
    ListStartChance = (int)((double)GetConfigFloat("test.list-start-chance", 0) * (double)RAND_MAX);
    ListEndChance   = (int)((double)GetConfigFloat("test.list-end-chance", 0.1) * (double)RAND_MAX);
    srand((int)time(NULL));
    return RunTests();
}
