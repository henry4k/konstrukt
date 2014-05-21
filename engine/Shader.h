#ifndef __APOAPSIS_SHADER__
#define __APOAPSIS_SHADER__

#include "OpenGL.h"
#include "Math.h"


typedef GLuint ShaderObject;
typedef GLuint ShaderProgram;

static const ShaderObject INVALID_SHADER_OBJECT = 0;
static const ShaderProgram INVALID_SHADER_PROGRAM = 0;


/**
 * Creates a shader object by reading the given `fileName`.
 * The object type is determined by the file extension automatically.
 *
 * @return
 * May return `INVALID_SHADER_OBJECT` if the type could not be determined or
 * if the shader source was erroneous.
 */
ShaderObject LoadShaderObject( const char* fileName );

void FreeShaderObject( ShaderObject object );

/**
 * Links the given `objects` into a shader program.
 *
 * @return
 * May return `INVALID_SHADER_PROGRAM` if linkage failed.
 */
void LinkShaderProgram( const ShaderObject* objects, int objectCount );

/**
 * @note
 * Shader objects used by the program should be freed before!
 */
void FreeShaderProgram( ShaderProgram program );

void BindShaderProgram( ShaderProgram program );

void SetUniform( ShaderProgram program, const char* name, int value );
void SetUniform( ShaderProgram program, const char* name, float value );
void SetUniform( ShaderProgram program, const char* name, int length, const float* value );
void SetUniformMatrix3( ShaderProgram program, const char* name, const glm::mat3* value );
void SetUniformMatrix4( ShaderProgram program, const char* name, const glm::mat4* value );

#endif
