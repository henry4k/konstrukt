#ifndef __SHADER__
#define __SHADER__

#include "OpenGL.h"

GLuint LoadShader( const char* vert, const char* frag );
void FreeShader( GLuint handle );
void BindShader( GLuint handle );
void SetUniform( GLuint shader, const char* name, int value );
void SetUniform( GLuint shader, const char* name, float value );
void SetUniform( GLuint shader, const char* name, int length, float* value );

#endif
