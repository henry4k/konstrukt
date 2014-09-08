#ifndef __APOAPSIS_SHADER__
#define __APOAPSIS_SHADER__

#include "Math.h"


static const int INVALID_UNIFORM_INDEX = -1;
static const int MAX_UNIFORM_NAME_LENGTH = 31;
static const int MAX_PROGRAM_FAMILY_NAME_LENGTH = 31;
static const int MAX_PROGRAM_FAMILY_LIST_LENGTH = 127;

struct Texture;
struct Shader;
struct ShaderProgram;
struct ShaderProgramSet;

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


bool InitShader();
void DestroyShader();

int GetUniformSize( UniformType type );

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

bool HasUniform( ShaderProgram* program, const char* name );

void SetUniform( ShaderProgram* program,
                 const char* name,
                 UniformType type,
                 const UniformValue* value );

void SetGlobalUniform( const char* name,
                       UniformType type,
                       const UniformValue* value );

void UnsetGlobalUniform( const char* name );

void ApplyGlobalUniforms( ShaderProgram* program );

ShaderProgram* GetShaderProgramByFamilyList( const ShaderProgramSet* set,
                                             const char* familyList );


#endif
