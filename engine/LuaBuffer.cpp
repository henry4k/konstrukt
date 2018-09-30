#include <assert.h>
#include <string.h> // strlen, memcpy

#include "Common.h"
#include "Array.h"
#include "Reference.h"
#include "LuaBuffer.h"


enum TokenType
{
    NIL_TOKEN,
    BOOLEAN_TOKEN,
    INTEGER_TOKEN,
    NUMBER_TOKEN,
    STRING_TOKEN
};

struct Token
{
    TokenType type;
    union
    {
        bool boolean;
        lua_Integer integer;
        lua_Number number;
        struct
        {
            int length;
            char* data;
        } string;
    } data;
};

struct LuaBuffer
{
    ReferenceCounter refCounter;
    LuaBufferType type;
    union
    {
        struct
        {
            Array<Token> tokens;
            Array<char> auxData;
        } native;
        Array<char> bytes;
    } data;
};

struct LuaBufferTypeDescription
{
    void (*initialize)(LuaBuffer* buffer);
    void (*destroy)(LuaBuffer* buffer);
    void (*addNil)(LuaBuffer* buffer);
    void (*addBoolean)(LuaBuffer* buffer, bool value);
    void (*addInteger)(LuaBuffer* buffer, lua_Integer value);
    void (*addNumber)(LuaBuffer* buffer, lua_Number value);
    void (*addString)(LuaBuffer* buffer, const char* string, int length);
    int  (*getData)(const LuaBuffer* buffer, const char** dataOut);
    int  (*pushToLua)(const char* data, int length, lua_State* state);
};


// --- native buffer

static void NativeBuffer_Initialize(LuaBuffer* buffer)
{
    InitArray(&buffer->data.native.tokens);
    InitArray(&buffer->data.native.auxData);
}

static void NativeBuffer_Destroy(LuaBuffer* buffer)
{
    DestroyArray(&buffer->data.native.tokens);
    DestroyArray(&buffer->data.native.auxData);
}

static Token* CreateToken(LuaBuffer* buffer, TokenType type)
{
    Token* token = AllocateAtEndOfArray(&buffer->data.native.tokens, 1);
    token->type = type;
    return token;
}

static void NativeBuffer_AddNil(LuaBuffer* buffer)
{
    CreateToken(buffer, NIL_TOKEN);
}

static void NativeBuffer_AddBoolean(LuaBuffer* buffer, bool value)
{
    Token* token = CreateToken(buffer, BOOLEAN_TOKEN);
    token->data.boolean = value;
}

static void NativeBuffer_AddInteger(LuaBuffer* buffer, lua_Integer value)
{
    Token* token = CreateToken(buffer, INTEGER_TOKEN);
    token->data.integer = value;
}

static void NativeBuffer_AddNumber(LuaBuffer* buffer, lua_Number value)
{
    Token* token = CreateToken(buffer, NUMBER_TOKEN);
    token->data.number = value;
}

static void NativeBuffer_AddString(LuaBuffer* buffer, const char* string, int length)
{
    Token* token = CreateToken(buffer, STRING_TOKEN);
    char* stringBuffer = AllocateAtEndOfArray(&buffer->data.native.auxData, length);
    memcpy(stringBuffer, string, length);
    token->data.string.length = length;
    token->data.string.data = stringBuffer;
}

static int NativeBuffer_GetData(const LuaBuffer* buffer, const char** dataOut)
{
    *dataOut = (const char*)buffer->data.native.tokens.data;
    return buffer->data.native.tokens.length * sizeof(Token);
}

static int NativeBuffer_PushToLua(const char* data, int length, lua_State* l)
{
    assert(length % sizeof(Token) == 0);
    const int tokenCount = length / sizeof(Token);
    const Token* tokens = (const Token*)data;
    REPEAT(tokenCount, i)
    {
        const Token* token = &tokens[i];
        switch(token->type)
        {
            case NIL_TOKEN:
                lua_pushnil(l);
                break;

            case BOOLEAN_TOKEN:
                lua_pushboolean(l, token->data.boolean);
                break;

            case INTEGER_TOKEN:
                lua_pushinteger(l, token->data.integer);
                break;

            case NUMBER_TOKEN:
                lua_pushinteger(l, token->data.number);
                break;

            case STRING_TOKEN:
                lua_pushlstring(l, token->data.string.data, token->data.string.length);
                break;
        }
    }
    return tokenCount; // TODO: Won't work with tables!
}


// --- general ---

static const LuaBufferTypeDescription BufferTypes[] =
{
    {
        NativeBuffer_Initialize,
        NativeBuffer_Destroy,
        NativeBuffer_AddNil,
        NativeBuffer_AddBoolean,
        NativeBuffer_AddInteger,
        NativeBuffer_AddNumber,
        NativeBuffer_AddString,
        NativeBuffer_GetData,
        NativeBuffer_PushToLua
    }
};

LuaBuffer* CreateLuaBuffer(LuaBufferType type)
{
    LuaBuffer* buffer = NEW(LuaBuffer);
    InitReferenceCounter(&buffer->refCounter);
    buffer->type = type;
    BufferTypes[type].initialize(buffer);
    return buffer;
}

static void FreeLuaBuffer(LuaBuffer* buffer)
{
    BufferTypes[buffer->type].destroy(buffer);
    FreeReferenceCounter(&buffer->refCounter);
    DELETE(buffer);
}

void ReferenceLuaBuffer( LuaBuffer* buffer )
{
    Reference(&buffer->refCounter);
}

void ReleaseLuaBuffer( LuaBuffer* buffer )
{
    Release(&buffer->refCounter);
    if(!HasReferences(&buffer->refCounter))
        FreeLuaBuffer(buffer);
}

void AddNilToLuaBuffer(LuaBuffer* buffer)
{
    BufferTypes[buffer->type].addNil(buffer);
}

void AddBooleanToLuaBuffer(LuaBuffer* buffer, bool value)
{
    BufferTypes[buffer->type].addBoolean(buffer, value);
}

void AddIntegerToLuaBuffer(LuaBuffer* buffer, lua_Integer value)
{
    BufferTypes[buffer->type].addInteger(buffer, value);
}

void AddNumberToLuaBuffer(LuaBuffer* buffer, lua_Number value)
{
    BufferTypes[buffer->type].addNumber(buffer, value);
}

void AddStringToLuaBuffer(LuaBuffer* buffer, const char* string, int length)
{
    assert(string != NULL && length >= 0);
    if(length == 0)
        length = strlen(string);
    BufferTypes[buffer->type].addString(buffer, string, length);
}

int PushLuaBufferToLua(LuaBuffer* buffer, lua_State* state)
{
    const char* data;
    const int length = GetLuaBufferData(buffer, &data);
    return PushLuaBufferDataToLua(buffer->type, data, length, state);
}

int GetLuaBufferData(const LuaBuffer* buffer, const char** dataOut)
{
    return BufferTypes[buffer->type].getData(buffer, dataOut);
}

int PushLuaBufferDataToLua(LuaBufferType type,
                              const char* data,
                              int length,
                              lua_State* state)
{
    assert(data != NULL && length > 0);
    return BufferTypes[type].pushToLua(data, length, state);
}
