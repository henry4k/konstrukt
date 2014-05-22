#ifndef __APOAPSIS_SHADER__
#define __APOAPSIS_SHADER__

#include "OpenGL.h"
#include "Math.h"


typedef GLuint Shader;
typedef GLuint ShaderProgram;

static const Shader INVALID_SHADER = 0;
static const ShaderProgram INVALID_SHADER_PROGRAM = 0;


/**
 * Creates a shader by reading the given `fileName`.
 * The shader type is determined by the file extension automatically.
 *
 * @return
 * May return `INVALID_SHADER` if the type could not be determined or
 * if the shader source was erroneous.
 */
Shader LoadShader( const char* fileName );

void FreeShader( Shader shader );

/**
 * Links the given `shaders` into a shader program.
 *
 * @return
 * May return `INVALID_SHADER_PROGRAM` if linkage failed.
 */
ShaderProgram LinkShaderProgram( const Shader* shaders, int shaderCount );

/**
 * @note
 * Shader shaders used by the program should be freed before!
 */
void FreeShaderProgram( ShaderProgram program );

void BindShaderProgram( ShaderProgram program );

void SetUniform( ShaderProgram program, const char* name, int value );
void SetUniform( ShaderProgram program, const char* name, float value );
void SetUniform( ShaderProgram program, const char* name, int length, const float* value );
void SetUniformMatrix3( ShaderProgram program, const char* name, const glm::mat3* value );
void SetUniformMatrix4( ShaderProgram program, const char* name, const glm::mat4* value );

#endif
