#ifndef __SHADER__
#define __SHADER__

#include "OpenGL.h"
#include "Math.h"

typedef GLuint Program;

Program LoadProgram( const char* vert, const char* frag );
void FreeProgram( Program program );
void BindProgram( Program program );
void SetUniform( Program program, const char* name, int value );
void SetUniform( Program program, const char* name, float value );
void SetUniform( Program program, const char* name, int length, const float* value );
void SetUniformMatrix3( Program program, const char* name, const glm::mat3* value );
void SetUniformMatrix4( Program program, const char* name, const glm::mat4* value );

#endif
