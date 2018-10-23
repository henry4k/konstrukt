#include <assert.h>
#include <string.h> // strlen, memcpy

#include "Common.h"
#include "Array.h"
#include "Reference.h"
#include "LuaBuffer.h"


//#define SIMPLE


enum TokenType
{
    NIL_TOKEN,
    BOOLEAN_TOKEN,
    INTEGER_TOKEN,
    NUMBER_TOKEN,
    STRING_TOKEN,
    USER_DATA_TOKEN,
#if defined(SIMPLE)
    LIST_START_TOKEN,
    LIST_END_TOKEN
#else
    LIST_TOKEN
#endif
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
            const char* data;
        } string;
        struct
        {
            int size;
            void* data;
        } userData;
#if !defined(SIMPLE)
        struct
        {
            int length;
        } list;
#endif
    } data;
};

#if !defined(SIMPLE)
struct Container
{
    int tokenIndex;
    int length;
};
#endif

struct NativeBuffer
{
    Array<Token> tokens;
    Array<char> auxData;
#if !defined(SIMPLE)
    Array<Container> containerStack; // used when adding nested containers
#endif
};

struct LuaBuffer
{
    ReferenceCounter refCounter;
    LuaBufferType type;
    // Implementation specific data will be allocated after this.
    // See CreateLuaBuffer
};

struct LuaBufferTypeDescription
{
    size_t size;
    void (*initialize)(void* buffer_);
    void (*destroy)(void* buffer_);
    void (*addNil)(void* buffer_);
    void (*addBoolean)(void* buffer_, bool value);
    void (*addInteger)(void* buffer_, lua_Integer value);
    void (*addNumber)(void* buffer_, lua_Number value);
    void (*addString)(void* buffer_,
                      const char* string,
                      int length,
                      int flags);
    void (*addUserData)(void* buffer_, void* value, int size);
    void (*beginList)(void* buffer_);
    void (*endList)(void* buffer_);
    void (*clear)(void* buffer_);
    int  (*getData)(const void* buffer_, const char** dataOut);
    int  (*pushToLua)(const char* data, int length, lua_State* state);
};


// --- native buffer

static void NativeBuffer_Initialize(void* buffer_)
{
    NativeBuffer* buffer = (NativeBuffer*)buffer_;
    InitArray(&buffer->tokens);
    InitArray(&buffer->auxData);
#if !defined(SIMPLE)
    InitArray(&buffer->containerStack);
#endif
}

static void NativeBuffer_Destroy(void* buffer_)
{
    NativeBuffer* buffer = (NativeBuffer*)buffer_;
    DestroyArray(&buffer->tokens);
    DestroyArray(&buffer->auxData);
#if !defined(SIMPLE)
    DestroyArray(&buffer->containerStack);
#endif
}

#if !defined(SIMPLE)
static Container* GetContainer(const NativeBuffer* buffer)
{
    Container* containers = buffer->containerStack.data;
    const int count = buffer->containerStack.length;
    if(count > 0)
        return &containers[count-1];
    else
        return NULL;
}
#endif

static Token* CreateToken(NativeBuffer* buffer, TokenType type)
{
    Token* token = AllocateAtEndOfArray(&buffer->tokens, 1);
    token->type = type;

#if !defined(SIMPLE)
    // Increment container counter:
    const int containerCount = buffer->containerStack.length;
    if(containerCount > 0)
    {
        Container* containers = buffer->containerStack.data;
        containers[containerCount-1].length++;
    }
#endif

    return token;
}

static void NativeBuffer_AddNil(void* buffer_)
{
    NativeBuffer* buffer = (NativeBuffer*)buffer_;
    CreateToken(buffer, NIL_TOKEN);
}

static void NativeBuffer_AddBoolean(void* buffer_, bool value)
{
    NativeBuffer* buffer = (NativeBuffer*)buffer_;
    Token* token = CreateToken(buffer, BOOLEAN_TOKEN);
    token->data.boolean = value;
}

static void NativeBuffer_AddInteger(void* buffer_, lua_Integer value)
{
    NativeBuffer* buffer = (NativeBuffer*)buffer_;
    Token* token = CreateToken(buffer, INTEGER_TOKEN);
    token->data.integer = value;
}

static void NativeBuffer_AddNumber(void* buffer_, lua_Number value)
{
    NativeBuffer* buffer = (NativeBuffer*)buffer_;
    Token* token = CreateToken(buffer, NUMBER_TOKEN);
    token->data.number = value;
}

static void NativeBuffer_AddString(void* buffer_,
                                   const char* string,
                                   int length,
                                   int flags)
{
    NativeBuffer* buffer = (NativeBuffer*)buffer_;
    Token* token = CreateToken(buffer, STRING_TOKEN);
    token->data.string.length = length;
    if(flags & LUA_BUFFER_LITERAL_STRING)
    {
        token->data.string.data = string;
    }
    else
    {
        char* stringBuffer = AllocateAtEndOfArray(&buffer->auxData, length);
        memcpy(stringBuffer, string, length);
        token->data.string.data = stringBuffer;
    }
}

static void NativeBuffer_AddUserData(void* buffer_, void* value, int size)
{
    NativeBuffer* buffer = (NativeBuffer*)buffer_;
    Token* token = CreateToken(buffer, USER_DATA_TOKEN);
    token->data.userData.size = size;
    if(size == 0)
    {
        token->data.userData.data = value;
    }
    else
    {
        void* dataBuffer = AllocateAtEndOfArray(&buffer->auxData, size);
        memcpy(dataBuffer, value, size);
        token->data.userData.data = dataBuffer;
    }
}

#if defined(SIMPLE)
static void NativeBuffer_BeginList(void* buffer_)
{
    NativeBuffer* buffer = (NativeBuffer*)buffer_;
    CreateToken(buffer, LIST_START_TOKEN);
}

static void NativeBuffer_EndList(void* buffer_)
{
    NativeBuffer* buffer = (NativeBuffer*)buffer_;
    CreateToken(buffer, LIST_END_TOKEN);
}
#else
static void NativeBuffer_BeginList(void* buffer_)
{
    NativeBuffer* buffer = (NativeBuffer*)buffer_;
    CreateToken(buffer, LIST_TOKEN);
    Container* container = AllocateAtEndOfArray(&buffer->containerStack, 1);
    container->tokenIndex = buffer->tokens.length - 1;
    container->length = 0;
}

static void NativeBuffer_EndList(void* buffer_)
{
    NativeBuffer* buffer = (NativeBuffer*)buffer_;
    Container* container = GetContainer(buffer);

    Token* token = GetArrayElement(&buffer->tokens,
                                   container->tokenIndex);
    assert(token->type == LIST_TOKEN);
    token->data.list.length = container->length;

    PopFromArray(&buffer->containerStack, 1);
}
#endif

static void NativeBuffer_Clear(void* buffer_)
{
    NativeBuffer* buffer = (NativeBuffer*)buffer_;
    ClearArray(&buffer->tokens);
    ClearArray(&buffer->auxData);
#if !defined(SIMPLE)
    ClearArray(&buffer->containerStack);
#endif
}

static int NativeBuffer_GetData(const void* buffer_, const char** dataOut)
{
    NativeBuffer* buffer = (NativeBuffer*)buffer_;
    *dataOut = (const char*)buffer->tokens.data;
    return buffer->tokens.length * sizeof(Token);
}

static void NativeBuffer_PushTokenToLua(lua_State* l,
                                        const Token* tokens,
                                        int tokenCount,
                                        int* tokenIndex)
{
    const Token* token = &tokens[*tokenIndex];
    (*tokenIndex)++;
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

        case USER_DATA_TOKEN:
            {
                const int size = token->data.userData.size;
                void* data = token->data.userData.data;
                if(size == 0)
                    lua_pushlightuserdata(l, data);
                else
                    memcpy(lua_newuserdata(l, size), data, size);
                break;
            }

        case STRING_TOKEN:
            lua_pushlstring(l, token->data.string.data, token->data.string.length);
            break;

#if defined(SIMPLE)
        case LIST_START_TOKEN:
        {
            lua_newtable(l);
            int luaIndex = 1;
            while(tokens[*tokenIndex].type != LIST_END_TOKEN)
            {
                NativeBuffer_PushTokenToLua(l, tokens, tokenCount, tokenIndex);
                lua_rawseti(l, -2, luaIndex);
                luaIndex++;
            }
            (*tokenIndex)++; // jump over the end token
            break;
        }

        case LIST_END_TOKEN:
            FatalError("This shouldn't be reachable!");
#else
        case LIST_TOKEN:
            lua_createtable(l, token->data.list.length, 0);
            for(int i = 1; i <= token->data.list.length; i++)
            {
                NativeBuffer_PushTokenToLua(l, tokens, tokenCount, tokenIndex);
                lua_rawseti(l, -2, i);
            }
            break;
#endif
    }
}

static int NativeBuffer_PushToLua(const char* data, int length, lua_State* l)
{
    assert(length % sizeof(Token) == 0);
    const int tokenCount = length / sizeof(Token);
    const Token* tokens = (const Token*)data;

    int pushedValues = 0;
    int tokenIndex = 0;
    do
    {
        NativeBuffer_PushTokenToLua(l, tokens, tokenCount, &tokenIndex);
        pushedValues++;
    } while(tokenIndex < tokenCount);

    return pushedValues;
}


// --- general ---

static const LuaBufferTypeDescription BufferTypes[] =
{
    {
        sizeof(NativeBuffer),
        NativeBuffer_Initialize,
        NativeBuffer_Destroy,
        NativeBuffer_AddNil,
        NativeBuffer_AddBoolean,
        NativeBuffer_AddInteger,
        NativeBuffer_AddNumber,
        NativeBuffer_AddString,
        NativeBuffer_AddUserData,
        NativeBuffer_BeginList,
        NativeBuffer_EndList,
        NativeBuffer_Clear,
        NativeBuffer_GetData,
        NativeBuffer_PushToLua
    }
};

static void* BufferImpl(LuaBuffer* buffer)
{
    return buffer + 1;
}

static const void* ConstBufferImpl(const LuaBuffer* buffer)
{
    return buffer + 1;
}

LuaBuffer* CreateLuaBuffer(LuaBufferType type)
{
    LuaBuffer* buffer = (LuaBuffer*)Alloc(sizeof(LuaBuffer) + BufferTypes[type].size);
    InitReferenceCounter(&buffer->refCounter);
    buffer->type = type;
    BufferTypes[type].initialize(BufferImpl(buffer));
    return buffer;
}

static void FreeLuaBuffer(LuaBuffer* buffer)
{
    BufferTypes[buffer->type].destroy(BufferImpl(buffer));
    FreeReferenceCounter(&buffer->refCounter);
    Free(buffer);
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
    BufferTypes[buffer->type].addNil(BufferImpl(buffer));
}

void AddBooleanToLuaBuffer(LuaBuffer* buffer, bool value)
{
    BufferTypes[buffer->type].addBoolean(BufferImpl(buffer), value);
}

void AddIntegerToLuaBuffer(LuaBuffer* buffer, lua_Integer value)
{
    BufferTypes[buffer->type].addInteger(BufferImpl(buffer), value);
}

void AddNumberToLuaBuffer(LuaBuffer* buffer, lua_Number value)
{
    BufferTypes[buffer->type].addNumber(BufferImpl(buffer), value);
}

void AddStringToLuaBuffer(LuaBuffer* buffer,
                          const char* string,
                          int length,
                          int flags)
{
    assert(string != NULL && length >= 0);
    if(length == 0)
        length = strlen(string);
    BufferTypes[buffer->type].addString(BufferImpl(buffer), string, length, flags);
}

void AddUserDataToLuaBuffer(LuaBuffer* buffer, void* value, int size)
{
    if(!value)
        assert(size == 0);
    else
        assert(size >= 0);
    BufferTypes[buffer->type].addUserData(BufferImpl(buffer), value, size);
}

void BeginListInLuaBuffer(LuaBuffer* buffer)
{
    BufferTypes[buffer->type].beginList(BufferImpl(buffer));
}

void EndListInLuaBuffer(LuaBuffer* buffer)
{
    BufferTypes[buffer->type].endList(BufferImpl(buffer));
}

void ClearLuaBuffer(LuaBuffer* buffer)
{
    BufferTypes[buffer->type].clear(BufferImpl(buffer));
}

int PushLuaBufferToLua(LuaBuffer* buffer, lua_State* state)
{
    const char* data;
    const int length = GetLuaBufferData(buffer, &data);
    if(data && length > 0)
        return PushLuaBufferDataToLua(buffer->type, data, length, state);
    else
        return 0;
}

int GetLuaBufferData(const LuaBuffer* buffer, const char** dataOut)
{
    return BufferTypes[buffer->type].getData(ConstBufferImpl(buffer), dataOut);
}

int PushLuaBufferDataToLua(LuaBufferType type,
                              const char* data,
                              int length,
                              lua_State* state)
{
    assert(data != NULL && length > 0);
    return BufferTypes[type].pushToLua(data, length, state);
}
