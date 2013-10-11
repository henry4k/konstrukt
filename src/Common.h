#ifndef __COMMON__
#define __COMMON__

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <assert.h>

typedef unsigned int Handle;

void FatalError( const char* format, ... );

#endif
