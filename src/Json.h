#ifndef __JSON__
#define __JSON__

#include <stdio.h>
#include "Squirrel.h"

/**
 * Parses the given string as JSON document
 * and constructs a squirrel object from it.
 *
 * @return
 * On success it returns `true` and pushes a table in the stack.
 * On failure it returns `false` and pushes an error message in the stack.
 */
bool ParseJsonString( HSQUIRRELVM vm, const char* str );

/**
 * Reads the given file and parses its contents as JSON document
 * and constructs a squirrel object from it.
 *
 * @return
 * On success it returns `true` and pushes a table in the stack.
 * On failure it returns `false` and pushes an error message in the stack.
 */
bool ParseJsonFile( HSQUIRRELVM vm, FILE* file );

/**
 * Creates a JSON string from the object at `stackPosition`.
 *
 * @param stackPosition
 * Squirrel object that is serialized.
 *
 * @param pretty
 * Pretty format json document.
 *
 * @return
 * On success it returns `true` and pushes the JSON string in the stack.
 * On failure it returns `false` and pushes an error message in the stack.
 */
bool WriteJsonString( HSQUIRRELVM vm, int stackPosition, bool pretty );

/**
 * Writes the object at `stackPosition` as JSON document to the given file.
 *
 * @param stackPosition
 * Squirrel object that is serialized.
 *
 * @param pretty
 * Pretty format json document.
 *
 * @param file
 * File where the JSON document is written to.
 *
 * @return
 * On success it returns `true`.
 * On failure it returns `false` and pushes an error message in the stack.
 */
bool WriteJsonFile( HSQUIRRELVM vm, int stackPosition, bool pretty, FILE* file );


#endif
