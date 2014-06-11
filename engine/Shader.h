#ifndef __APOAPSIS_SHADER__
#define __APOAPSIS_SHADER__

#include "OpenGL.h"
#include "Math.h"


typedef GLuint Shader;

static const Shader INVALID_SHADER = 0;
static const int INVALID_UNIFORM_INDEX = -1;
static const int MAX_UNIFORM_NAME_LENGTH = 32;

enum UniformType
{
    INT_UNIFORM,
    FLOAT_UNIFORM,
    VEC3_UNIFORM,
    VEC4_UNIFORM,
    MAT3_UNIFORM,
    MAT4_UNIFORM
};

struct UniformValue
{
    const float& f() const;
    const glm::vec3& v3() const;
    const glm::vec4& v4() const;
    const glm::mat3& m3() const;
    const glm::mat4& m4() const;

    float& f();
    glm::vec3& v3();
    glm::vec4& v4();
    glm::mat3& m3();
    glm::mat4& m4();

    float data[4*4];
};

struct UniformDefinition;

struct ShaderProgram;


/**
 * Creates a shader by reading the given `fileName`.
 * The shader type is determined by the file extension automatically.
 *
 * @return
 * May return #INVALID_SHADER if the type could not be determined or
 * if the shader source was erroneous.
 */
Shader LoadShader( const char* fileName );

void FreeShader( Shader shader );

/**
 * Links the given `shaders` into a shader program.
 *
 * @return
 * `NULL` if and error occured during linkage.
 */
ShaderProgram* LinkShaderProgram( const Shader* shaders, int shaderCount );

/**
 * @note
 * Shaders used by the program should be freed before!
 */
void FreeShaderProgram( ShaderProgram* program );

void BindShaderProgram( const ShaderProgram* program );

int GetUniformCount( const ShaderProgram* program );

/**
 * @return
 * The uniforms index or #INVALID_UNIFORM_INDEX.
 */
int GetUniformIndex( const ShaderProgram* program, const char* name );

void SetUniform( ShaderProgram* program, int index, const UniformValue* value );
void SetUniformDefault( ShaderProgram* program, int index, const UniformValue* value );
void ResetUniform( ShaderProgram* program, int index );

void SetUniform( ShaderProgram* program, const char* name, const UniformValue* value );
void SetUniformDefault( ShaderProgram* program, const char* name, const UniformValue* value );
void ResetUniform( ShaderProgram* program, const char* name );

#endif
