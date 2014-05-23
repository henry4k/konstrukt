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
    FLOAT_UNIFORM,
    VEC3_UNIFORM,
    VEC4_UNIFORM,
    MAT3_UNIFORM,
    MAT4_UNIFORM
};

union UniformValue
{
    float f;
    glm::vec3 v3;
    glm::vec4 v4;
    glm::mat3 m3;
    glm::mat4 m4;
};

struct UniformDescription
{
    char name[MAX_UNIFORM_NAME_LENGTH];
    int location;
    UniformType type;
};

struct ShaderProgram
{
    GLuint handle;
    int uniformCount;
    UniformDescription* uniformDescriptions;
    UniformValue* defaultUniformValues;
};


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
 */
bool LinkShaderProgram( ShaderProgram* program, const Shader* shaders, int shaderCount );

/**
 * @note
 * Shaders used by the program should be freed before!
 */
void FreeShaderProgram( ShaderProgram* program );

void BindShaderProgram( ShaderProgram* program );

/**
 * @return
 * The uniforms index or #INVALID_UNIFORM_INDEX.
 */
int GetUniformIndex( ShaderProgram* program, const char* name )


void SetUniformValueInShaderProgram( ShaderProgram* program, int index, UniformValue* value );

#endif
