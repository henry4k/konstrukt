#include <string.h>
#include <vector>
#include <json.h> // TODO: Seems a bit dangerous. Maybe use <thirdparty/libjson/json.h> for clarity instaed?

#include "Common.h"
#include "Squirrel.h"
#include <sqstdio.h> // Needed by sqstd_getfile and SQFILE
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
    json_parser parser;
    json_config config;

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

bool InitJsonParserContext( JsonParserContext* context, HSQUIRRELVM vm )
{
    memset(&context, 0, sizeof(JsonParserContext));
    context->vm = vm;

    InitJsonConfig(&context->config);

    int error = json_parser_init(&context->parser, &context->config, JsonParserCallback, &context);
    if(error)
    {
        const char* errorString = Format("During parser initialization: %s",
            JsonErrorToString(error)
        );
        sq_pushstring(vm, errorString, -1);
        return false;
    }

    return true;
}

bool FreeJsonParserContext( JsonParserContext* context )
{
    if(!json_parser_is_done(&context->parser))
    {
        sq_pushstring(context->vm, "Parser expects more characters.", -1);
        json_parser_free(&context->parser);
        return false;
    }

    json_parser_free(&context->parser);
    return true;
}

bool FeedJsonParser( JsonParserContext* context, const char* buffer, int length )
{
    uint32_t processedChars = 0;
    int error = json_parser_string(&context->parser, buffer, length, &processedChars);
    if(error)
    {
        // TODO: Recognize lines.
        const char* errorString = Format("At character %d: %s",
            processedChars,
            JsonErrorToString(error)
        );
        sq_pushstring(context->vm, errorString, -1);
        json_parser_free(&context->parser);
        return false;
    }
    return true;
}

bool ParseJsonString( HSQUIRRELVM vm, const char* str )
{
    JsonParserContext context;
    if(!InitJsonParserContext(&context, vm))
        return false;

    if(!FeedJsonParser(&context, str, strlen(str)))
        return false;

    return FreeJsonParserContext(&context);
}

bool ParseJsonFile( HSQUIRRELVM vm, FILE* file )
{
    JsonParserContext context;
    if(!InitJsonParserContext(&context, vm))
        return false;

    static const int bufferSize = 1024;
    char buffer[bufferSize];
    while(!feof(file))
    {
        const int charsRead = fread(buffer, 1, bufferSize, file);
        if(!FeedJsonParser(&context, buffer, charsRead))
            return false;
    }

    return FreeJsonParserContext(&context);
}

// --- Writer ---

struct JsonStringPrinterContext
{
    std::vector<char> buffer;
};

int JsonStringPrinterCallback( void* userdata, const char* data, uint32_t length )
{
    JsonStringPrinterContext* context = (JsonStringPrinterContext*)userdata;

    context->buffer.insert(
        context->buffer.end(),
        data,
        data+length
    );

    return 0;
}

struct JsonFilePrinterContext
{
    FILE* file;
};

int JsonFilePrinterCallback( void* userdata, const char* data, uint32_t length )
{
    JsonFilePrinterContext* context = (JsonFilePrinterContext*)userdata;

    const int writtenChars = fwrite(data, 1, length, context->file);
    assert(writtenChars == length);

    return 0;
}

struct JsonTokenizerContext
{
    HSQUIRRELVM vm;
    json_printer* printer;
    bool pretty;
};

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
bool WriteJsonToken( JsonTokenizerContext* context, int type, const char* data, int length )
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

bool WriteSquirrelObject( JsonTokenizerContext* context, int stackPosition )
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

/**
 * Serializes a JSON document from the squirrel object at `stackPosition`.
 *
 * @param stackPosition
 * Squirrel object that is serialized.
 *
 * @param pretty
 * Pretty format json document.
 *
 * @param printerCallback
 * Callback for printing the JSON document.
 *
 * @param printerContext
 * Context value thats passed to the printerCallback.
 *
 * @return
 * On success it returns `true`.
 * On failure it returns `false` and pushes an error message in the stack.
 */
bool WriteJson( HSQUIRRELVM vm, int stackPosition, bool pretty, json_printer_callback printerCallback, void* printerContext )
{
    json_printer printer;

    int error = 0;

    JsonTokenizerContext tokenizerContext;
    tokenizerContext.vm = vm;
    tokenizerContext.printer = &printer;
    tokenizerContext.pretty = pretty;

    error = json_print_init(&printer, printerCallback, printerContext);
    if(error)
    {
        const char* errorString = Format("During printer initialization: %s",
            JsonErrorToString(error)
        );
        sq_pushstring(vm, errorString, -1);
        return false;
    }

    if(!WriteSquirrelObject(&tokenizerContext, stackPosition))
    {
        json_print_free(&printer);
        return false;
    }

    json_print_free(&printer);
    return true;
}

bool WriteJsonString( HSQUIRRELVM vm, int stackPosition, bool pretty )
{
    JsonStringPrinterContext context;
    if(!WriteJson(vm, stackPosition, pretty, JsonStringPrinterCallback, &context))
        return false;
    sq_pushstring(vm, &context.buffer[0], context.buffer.size());
    return true;
}

bool WriteJsonFile( HSQUIRRELVM vm, int stackPosition, bool pretty, FILE* file )
{
    JsonFilePrinterContext context;
    context.file = file;
    return WriteJson(vm, stackPosition, pretty, JsonFilePrinterCallback, &context);
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

SQInteger Squirrel_ParseJsonFile( HSQUIRRELVM vm )
{
    SQFILE file;
    sqstd_getfile(vm, 2, &file);

    if(ParseJsonFile(vm, (FILE*)file))
        return 1;
    else
        return sq_throwobject(vm);
}
RegisterStaticFunctionInSquirrel(ParseJsonFile, 2, ".x");

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

SQInteger Squirrel_WriteJsonFile( HSQUIRRELVM vm )
{
    SQBool pretty = SQFalse;
    sq_getbool(vm, 3, &pretty);

    SQFILE file;
    sqstd_getfile(vm, 4, &file);

    if(WriteJsonFile(vm, 2, (bool)pretty, (FILE*)file))
        return 1;
    else
        return sq_throwobject(vm);
}
RegisterStaticFunctionInSquirrel(WriteJsonFile, 4, "..bx");
