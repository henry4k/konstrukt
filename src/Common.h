#ifndef __COMMON__
#define __COMMON__

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <assert.h>

const char* Format( const char* format, ... );
void Log( const char* format, ... );
void Error( const char* format, ... );
void FatalError( const char* format, ... );

#endif
