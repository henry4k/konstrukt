#ifndef __SHADER__
#define __SHADER__

#include "Common.h"

Handle LoadShader( const char* vert, const char* frag );
void FreeShader( Handle handle );
void BindShader( Handle handle );
void SetUniform( Handle shader, const char* name, int value );
void SetUniform( Handle shader, const char* name, float value );
void SetUniform( Handle shader, const char* name, int length, float* value );

#endif
