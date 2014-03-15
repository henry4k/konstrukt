#include <string.h>
#include <vector>
#include <json.h> // TODO: Seems a bit dangerous. Maybe use <thirdparty/libjson/json.h> for clarity instaed?
#include "Common.h"
#include "Json.h"

const char* JsonErrorToString( int error )
{
    switch(error)
    {
        case 0:
            return NULL;
        case JSON_ERROR_NO_MEMORY:
            return "Running out of memory.";
        case JSON_ERROR_BAD_CHAR:
            return "Character < 32, except space, newline and tab.";
        case JSON_ERROR_POP_EMPTY:
            return "Trying to pop more object/array than pushed on the stack.";
        case JSON_ERROR_POP_UNEXPECTED_MODE:
            return "Trying to pop wrong type of mode. popping array in object mode, vice versa.";
        case JSON_ERROR_NESTING_LIMIT:
            return "Reach nesting limit on stack.";
        case JSON_ERROR_DATA_LIMIT:
            return "Reach data limit on buffer.";
        case JSON_ERROR_COMMENT_NOT_ALLOWED:
            return "Comment are not allowed with current configuration.";
        case JSON_ERROR_UNEXPECTED_CHAR:
            return "Unexpected char in the current parser context.";
        case JSON_ERROR_UNICODE_MISSING_LOW_SURROGATE:
            return "Unicode low surrogate missing after high surrogate.";
        case JSON_ERROR_UNICODE_UNEXPECTED_LOW_SURROGATE:
            return "Unicode low surrogate missing without previous high surrogate.";
        case JSON_ERROR_COMMA_OUT_OF_STRUCTURE:
            return "Found a comma not in structure (array/object).";
        case JSON_ERROR_CALLBACK:
            return "Callback returns error.";
        case JSON_ERROR_UTF8:
            return "utf8 stream is invalid.";
        default:
            return "Unknown error.";
    }
}

const int JsonParserStackSize = LIBJSON_DEFAULT_STACK_SIZE;

void InitJsonConfig( json_config* c )
{
    memset(c, 0, sizeof(json_config));
    c->buffer_initial_size = 0;
    c->max_nesting = JsonParserStackSize;
    c->max_data = 0;
    c->allow_c_comments = 1;
    c->allow_yaml_comments = 0;
    c->user_calloc = NULL;
    c->user_realloc = NULL;
}


// --- Parser ---

struct JsonParserContext
{
    HSQUIRRELVM vm;
    char stack[JsonParserStackSize]; // 0 = Array; 1 = Table
    int stackPosition;
};

int JsonParserCallback( void* userdata, int type, const char* data, uint32_t length )
{
    JsonParserContext* context = (JsonParserContext*)userdata;
    HSQUIRRELVM vm = context->vm;

    switch(type)
    {
        case JSON_ARRAY_BEGIN:
            sq_newarray(vm, 0);
            context->stack[context->stackPosition] = 0;
            context->stackPosition++;
            return 0;

        case JSON_OBJECT_BEGIN:
            sq_newtableex(vm, 0);
            context->stack[context->stackPosition] = 1;
            context->stackPosition++;
            return 0;

        case JSON_ARRAY_END:
            context->stackPosition--;
            if(context->stackPosition < 0)
            {
                FatalError("Stack position below zero.");
                //context->stackPosition = 0;
                return 0;
            }
            break;

        case JSON_OBJECT_END:
            context->stackPosition--;
            if(context->stackPosition < 0)
            {
                FatalError("Stack position below zero.");
                //context->stackPosition = 0;
                return 0;
            }
            break;

        case JSON_INT:
            {
                char* end = NULL;
                const long value = strtol(data, &end, 10);
                sq_pushinteger(vm, value);
            }
            break;

        case JSON_FLOAT:
            {
                char* end = NULL;
                const double value = strtod(data, &end);
                sq_pushfloat(vm, value);
            }
            break;

        case JSON_STRING:
        case JSON_BSTRING:
            sq_pushstring(vm, data, length);
            break;

        case JSON_KEY:
            sq_pushstring(vm, data, length);
            return 0;

        case JSON_TRUE:
            sq_pushbool(vm, SQTrue);
            break;

        case JSON_FALSE:
            sq_pushbool(vm, SQFalse);
            break;

        case JSON_NULL:
            sq_pushnull(vm);
            break;

        case JSON_NONE:
            FatalError("JSON_NONE");
            return 0;

        default:
            FatalError("Unknown JSON token type %d.", type);
            return 0;
    }

    if(context->stack[context->stackPosition] == 0) // Object ?
        sq_newslot(vm, -3, SQFalse);
    else // Array !
        sq_arrayappend(vm, -2);

    return 0;
}

bool ParseJsonString( HSQUIRRELVM vm, const char* str )
{
    json_parser parser;
    json_config config;
    InitJsonConfig(&config);

    int error = 0;

    JsonParserContext context;
    memset(&context, 0, sizeof(JsonParserContext));
    context.vm = vm;

    error = json_parser_init(&parser, &config, JsonParserCallback, &context);
    if(error)
    {
        const char* errorString = Format("During parser initialization: %s",
            JsonErrorToString(error)
        );
        sq_pushstring(vm, errorString, -1);
        return false;
    }

    uint32_t chars = strlen(str);
    uint32_t processedChars = 0;
    error = json_parser_string(&parser, str, chars, &processedChars);
    if(error)
    {
        // TODO: Recognize lines.
        const char* errorString = Format("At character %d: %s",
            processedChars,
            JsonErrorToString(error)
        );
        sq_pushstring(vm, errorString, -1);
        json_parser_free(&parser);
        return false;
    }

    if(!json_parser_is_done(&parser))
    {
        sq_pushstring(vm, "Parser expects more characters.", -1);
        json_parser_free(&parser);
        return false;
    }

    json_parser_free(&parser);
    return true;
}


// --- Writer ---

struct JsonPrinterContext
{
    HSQUIRRELVM vm;
    std::vector<char> buffer;

    json_printer* printer;
    bool pretty;
};

int JsonPrinterCallback( void* userdata, const char* data, uint32_t length )
{
    JsonPrinterContext* context = (JsonPrinterContext*)userdata;
    HSQUIRRELVM vm = context->vm;

    context->buffer.insert(
        context->buffer.end(),
        data,
        data+length
    );

    return 0;
}

/**
 * Takes token information and passes it to the printer.
 *
 * @param data
 * String content of the token.
 *
 * @param length
 * Length of the `data` string.
 * If less than 0 string length will be calculated automatically.
 */
bool WriteJsonToken( JsonPrinterContext* context, int type, const char* data, int length )
{
    if(length < 0)
        length = strlen(data);

    int error = 0;
    if(context->pretty)
        error = json_print_pretty(context->printer, type, data, length);
    else
        error = json_print_raw(context->printer, type, data, length);

    if(error)
    {
        const char* errorString = Format("While writing json data: %s",
            JsonErrorToString(error)
        );
        sq_pushstring(context->vm, errorString, -1);
        return false;
    }

    return true;
}

bool WriteSquirrelObject( JsonPrinterContext* context, int stackPosition )
{
    HSQUIRRELVM vm = context->vm;

    switch(sq_gettype(vm, stackPosition))
    {
        case OT_NULL:
            return WriteJsonToken(context, JSON_NULL, NULL, 0);

        case OT_BOOL:
            {
                SQBool value = false;
                sq_getbool(vm, stackPosition, &value);
                return WriteJsonToken(context, value ? JSON_TRUE : JSON_FALSE, NULL, 0);
            }

        case OT_INTEGER:
            {
                SQInteger value = 0;
                sq_getinteger(vm, stackPosition, &value);
                return WriteJsonToken(context, JSON_INT, Format("%d", value), -1);
            }

        case OT_FLOAT:
            {
                SQFloat value = 0;
                sq_getfloat(vm, stackPosition, &value);
                return WriteJsonToken(context, JSON_FLOAT, Format("%g", value), -1);
            }

        case OT_STRING:
            {
                const SQChar* value;
                sq_getstring(vm, stackPosition, &value);
                return WriteJsonToken(context, JSON_STRING, value, -1);
            }

        case OT_TABLE:
        {
            if(!WriteJsonToken(context, JSON_OBJECT_BEGIN, NULL, 0))
                return false;

            sq_pushnull(vm); // Push interator
            while(SQ_SUCCEEDED(sq_next(vm, stackPosition)))
            {
                // Key
                const SQChar* key;
                sq_getstring(vm, -2, &key);
                if(!WriteJsonToken(context, JSON_KEY, key, -1))
                    return false;

                // Value
                if(!WriteSquirrelObject(context, sq_gettop(vm)))
                    return false;

                sq_pop(vm, 2); // Pop key and value
            }
            sq_pop(vm, 1); // Pop iterator

            return WriteJsonToken(context, JSON_OBJECT_END, NULL, 0);
        }

        case OT_ARRAY:
        {
            if(!WriteJsonToken(context, JSON_ARRAY_BEGIN, NULL, 0))
                return false;

            sq_pushnull(vm); // Push interator
            while(SQ_SUCCEEDED(sq_next(vm, stackPosition)))
            {
                // Value
                if(!WriteSquirrelObject(context, sq_gettop(vm)))
                    return false;

                sq_pop(vm, 2); // Pop value
            }
            sq_pop(vm, 1); // Pop iterator

            return WriteJsonToken(context, JSON_ARRAY_END, NULL, 0);
        }

        default:
            sq_pushstring(vm, "Unserializable squirrel value.", -1);
            return false;
    }
}

bool WriteJsonString( HSQUIRRELVM vm, int stackPosition, bool pretty )
{
    json_printer printer;

    int error = 0;

    JsonPrinterContext context;
    context.vm = vm;
    context.printer = &printer;
    context.pretty = pretty;

    error = json_print_init(&printer, JsonPrinterCallback, &context);
    if(error)
    {
        const char* errorString = Format("During printer initialization: %s",
            JsonErrorToString(error)
        );
        sq_pushstring(vm, errorString, -1);
        return false;
    }

    if(!WriteSquirrelObject(&context, stackPosition))
    {
        json_print_free(&printer);
        return false;
    }

    sq_pushstring(vm, &context.buffer[0], context.buffer.size());
    json_print_free(&printer);
    return true;
}


// --- Squirrel Bindings ---

SQInteger Squirrel_ParseJsonString( HSQUIRRELVM vm )
{
    const char* str = NULL;
    sq_getstring(vm, 2, &str);

    if(ParseJsonString(vm, str))
        return 1;
    else
        return sq_throwobject(vm);
}
RegisterStaticFunctionInSquirrel(ParseJsonString, 2, ".s");

SQInteger Squirrel_WriteJsonString( HSQUIRRELVM vm )
{
    SQBool pretty = SQFalse;
    sq_getbool(vm, 3, &pretty);

    if(WriteJsonString(vm, 2, (bool)pretty))
        return 1;
    else
        return sq_throwobject(vm);
}
RegisterStaticFunctionInSquirrel(WriteJsonString, 3, "..b");
