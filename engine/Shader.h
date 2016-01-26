#ifndef __APOAPSIS_SHADER__
#define __APOAPSIS_SHADER__

#include "Math.h"

struct Texture;


static const int INVALID_UNIFORM_INDEX = -1;
static const int MAX_UNIFORM_NAME_SIZE = 32;
static const int MAX_PROGRAM_FAMILY_SIZE = 32;
static const int MAX_PROGRAM_FAMILY_LIST_SIZE = 128;


/**
 * Shaders are compiled shader sources, like objects are compiled `.c/.cpp`
 * sources.
 *
 * Shaders alone aren't of any use.  They first need to be linked to into a
 * #ShaderProgram, which can then be used for rendering by the graphics driver.
 *
 * This is just a brief summary, the complete documentation is available at
 * http://www.opengl.org/wiki/GLSL_Object
 */
struct Shader;

/**
 * Shader programs are used to define how models are rendered.
 *
 * They consist of multiple #Shader objects, which must provide an entry point
 * for each mandatory rendering stage:  Vertex and fragment shader.
 *
 * This is just a brief summary, the complete documentation is available at
 * http://www.opengl.org/wiki/GLSL_Object
 */
struct ShaderProgram;

/**
 * Some effects, like shadow mapping, require models to be rendered
 * with a special shader programs, that are adapted to the effect.
 *
 * Shader program sets group programs into families.
 * A model just needs to save the family name, which is then resolved by
 * the used shader program set.
 * E.g. there can be a family for static and one for animated models.
 */
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

typedef union
{
    int i;
    float f;
    Vec3 vec3;
    Vec4 vec4;
    Mat3 mat3;
    Mat4 mat4;
} UniformValue;


bool InitShader();
void DestroyShader();

int GetUniformSize( UniformType type );

/**
 * Creates a shader by reading the given `vfsPath`.
 * The shader type is determined by the file extension automatically.
 *
 * @return
 * May return `NULL` if the type could not be determined or
 * if the shader source was erroneous.
 */
Shader* LoadShader( const char* vfsPath );

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

/**
 * Use the given program for future render operations.
 */
void BindShaderProgram( ShaderProgram* program );

bool HasUniform( const ShaderProgram* program, const char* name );

void SetUniform( ShaderProgram* program,
                 const char* name,
                 UniformType type,
                 const UniformValue* value );

/**
 * Sets the uniform `name` in all shader programs.
 */
void SetGlobalUniform( const char* name,
                       UniformType type,
                       const UniformValue* value );

/**
 * Frees the global uniform slot, but program uniforms are *not* updated.
 */
void UnsetGlobalUniform( const char* name );


/**
 * @param defaultProgram
 * The default program is used by #GetShaderProgramByFamilyList if a family
 * list yields no matches.
 */
ShaderProgramSet* CreateShaderProgramSet( ShaderProgram* defaultProgram );

void ReferenceShaderProgramSet( ShaderProgramSet* set );
void ReleaseShaderProgramSet( ShaderProgramSet* set );

void SetShaderProgramFamily( ShaderProgramSet* set,
                             const char* family,
                             ShaderProgram* program );

/**
 * Searches a #ShaderProgramSet for the best matching entry.
 *
 * It will iterate through each family name and search the set for it.
 * 1. If a matching entry is found, the program will be returned.
 * 2. If no family matched, it uses the default #ShaderProgram.
 *
 * @param familyList
 * A list of family names, separated by commas.
 * E.g. `animated,static`
 */
ShaderProgram* GetShaderProgramByFamilyList( const ShaderProgramSet* set,
                                             const char* familyList );

#endif
