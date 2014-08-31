#ifndef __APOAPSIS_SHADER__
#define __APOAPSIS_SHADER__

#include "Math.h"


static const int INVALID_UNIFORM_INDEX = -1;
static const int MAX_UNIFORM_NAME_LENGTH = 32;

struct Texture;
struct Shader;
struct ShaderProgram;

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
    union
    {
        int i;
        float f;
        float data[4*4];
    };

    const glm::vec3& v3() const;
    const glm::vec4& v4() const;
    const glm::mat3& m3() const;
    const glm::mat4& m4() const;

    glm::vec3& v3();
    glm::vec4& v4();
    glm::mat3& m3();
    glm::mat4& m4();
};


/**
 * Creates a shader by reading the given `fileName`.
 * The shader type is determined by the file extension automatically.
 *
 * @return
 * May return `NULL` if the type could not be determined or
 * if the shader source was erroneous.
 */
Shader* LoadShader( const char* fileName );

void ReferenceShader( Shader* shader );
void ReleaseShader( Shader* shader );

/**
 * Links the given `shaders` into a shader program.
 *
 * @return
 * `NULL` if and error occured during linkage.
 */
ShaderProgram* LinkShaderProgram( Shader** shaders, int shaderCount );

void ReferenceShaderProgram( ShaderProgram* program );
void ReleaseShaderProgram( ShaderProgram* program );

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
