#include "../LuaBuffer.h"
#include "TestTools.h"


InlineTest("Basic Test")
{
    LuaBuffer* buffer = CreateLuaBuffer(NATIVE_LUA_BUFFER);
    ReferenceLuaBuffer(buffer);

    AddNilToLuaBuffer(buffer);
    AddBooleanToLuaBuffer(buffer, true);
    AddIntegerToLuaBuffer(buffer, 42);
    AddNumberToLuaBuffer(buffer, 13.37);
    AddStringToLuaBuffer(buffer, "foo", 0);
    AddStringToLuaBuffer(buffer, "bar", 3);
    const char* data;
    const int length = GetLuaBufferData(buffer, &data);
    Require(length > 0);
    Require(data != NULL);

    ReleaseLuaBuffer(buffer);
}


int main( int argc, char** argv )
{
    InitTests(argc, argv);
    return RunTests();
}
