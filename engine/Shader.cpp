#include <assert.h>
#include <string.h> // strncmp, strlen, memcmp, memset

#include "Common.h"
#include "OpenGL.h"
#include "Vertex.h"
#include "Reference.h"
#include "Shader.h"


static const GLuint INVALID_SHADER_HANDLE = 0;
static const int MAX_GLOBAL_UNIFORMS = 16;
static const int MAX_SHADER_PROGRAM_SET_ENTRIES = 16;


struct Shader
{
    ReferenceCounter refCounter;
    GLuint handle;
};

struct UniformDefinition
{
    char name[MAX_UNIFORM_NAME_SIZE];
    int location;
    UniformType type;
};

struct ShaderProgram
{
    ReferenceCounter refCounter;

    GLuint handle;

    int shaderCount;
    Shader** shaders;

    int uniformCount;
    UniformDefinition* uniformDefinitions;
    UniformValue* currentUniformValues;
};

struct ShaderProgramSetEntry
{
    char family[MAX_PROGRAM_FAMILY_SIZE];
    ShaderProgram* program;
};

struct ShaderProgramSet
{
    ReferenceCounter refCounter;
    ShaderProgramSetEntry entries[MAX_SHADER_PROGRAM_SET_ENTRIES];
};

struct GlobalUniform
{
    bool active;
    char name[MAX_UNIFORM_NAME_SIZE];
    UniformType type;
    UniformValue value;
};

static GlobalUniform GlobalUniforms[MAX_GLOBAL_UNIFORMS];

bool InitShader()
{
    memset(GlobalUniforms, 0, sizeof(GlobalUniforms));
    return true;
}

void DestroyShader()
{
}


// ----- Shader ------

static void FreeShader( Shader* shader );

static void ShowShaderLog( Shader* shader, bool success )
{
    GLint length = 0;
    glGetShaderiv(shader->handle, GL_INFO_LOG_LENGTH, &length);

    char* log = NULL;
    if(length > 1) // Some drivers want me to log single newline characters.
    {
        log = new char[length];
        glGetShaderInfoLog(shader->handle, length, NULL, log);
    }

    if(log)
    {
        if(success)
            Log("%s", log);
        else
            Error("%s", log);
        delete[] log;
    }
}

static int ShaderTypeToGL( ShaderType type )
{
    switch(type)
    {
        case VERTEX_SHADER:   return GL_VERTEX_SHADER;
        case FRAGMENT_SHADER: return GL_FRAGMENT_SHADER;
    }
    FatalError("Unknown shader type %d", type);
    return 0;
}

Shader* CreateShader( ShaderType type, const char* source )
{
    Shader* shader = new Shader;
    InitReferenceCounter(&shader->refCounter);
    shader->handle = glCreateShader(ShaderTypeToGL(type));

    glShaderSource(shader->handle, 1, &source, NULL);

    glCompileShader(shader->handle);

    GLint state;
    glGetShaderiv(shader->handle, GL_COMPILE_STATUS, &state);
    if(state)
    {
        Log("Shader compiled successfully.");
        ShowShaderLog(shader, state);
        return shader;
    }
    else
    {
        Error("Error compiling shader:");
        ShowShaderLog(shader, state);
        FreeShader(shader);
        return NULL;
    }
}

static void FreeShader( Shader* shader )
{
    FreeReferenceCounter(&shader->refCounter);
    glDeleteShader(shader->handle);
    delete shader;
}

void ReferenceShader( Shader* shader )
{
    Reference(&shader->refCounter);
}

void ReleaseShader( Shader* shader )
{
    Release(&shader->refCounter);
    if(!HasReferences(&shader->refCounter))
        FreeShader(shader);
}


// ----- Shader Program -----

static void FreeShaderProgram( ShaderProgram* program );
static void ApplyGlobalUniforms( ShaderProgram* program );

static void ShowShaderProgramLog( ShaderProgram* program, bool success )
{
    GLint length = 0;
    glGetProgramiv(program->handle, GL_INFO_LOG_LENGTH, &length);

    char* log = NULL;
    if(length > 1) // See ShowShaderLog
    {
        log = new char[length];
        glGetProgramInfoLog(program->handle, length, NULL, log);
    }

    if(log)
    {
        if(success)
            Log("%s", log);
        else
            Error("%s", log);
        delete[] log;
    }
}

static UniformType GLToUniformType( GLenum glType )
{
    switch(glType)
    {
        case GL_INT:          return INT_UNIFORM;
        case GL_SAMPLER_1D:   return INT_UNIFORM;
        case GL_SAMPLER_2D:   return INT_UNIFORM;
        case GL_SAMPLER_3D:   return INT_UNIFORM;
        case GL_SAMPLER_CUBE: return INT_UNIFORM;
        case GL_FLOAT:        return FLOAT_UNIFORM;
        case GL_FLOAT_VEC3:   return VEC3_UNIFORM;
        case GL_FLOAT_VEC4:   return VEC4_UNIFORM;
        case GL_FLOAT_MAT3:   return MAT3_UNIFORM;
        case GL_FLOAT_MAT4:   return MAT4_UNIFORM;

        default:
            FatalError("Unsupported gl constant!");
            return FLOAT_UNIFORM;
    }
}

int GetUniformSize( UniformType type )
{
    switch(type)
    {
        case INT_UNIFORM:   return sizeof(int);
        case FLOAT_UNIFORM: return sizeof(float);
        case VEC3_UNIFORM:  return sizeof(Vec3);
        case VEC4_UNIFORM:  return sizeof(Vec4);
        case MAT3_UNIFORM:  return sizeof(Mat3);
        case MAT4_UNIFORM:  return sizeof(Mat4);
    }
    FatalError("Unknown UniformType!");
    return 0;
}

static void ReadUniformDefinitions( ShaderProgram* program )
{
    if(program->uniformDefinitions)
    {
        delete[] program->uniformDefinitions;
        program->uniformDefinitions = NULL;
    }

    if(program->currentUniformValues)
    {
        delete[] program->currentUniformValues;
        program->currentUniformValues = NULL;
    }

    int count = 0;
    glGetProgramiv(program->handle, GL_ACTIVE_UNIFORMS, &count);

    program->uniformCount = count;

    program->uniformDefinitions = new UniformDefinition[count];
    for(int i = 0; i < count; ++i)
    {
        UniformDefinition* def = &program->uniformDefinitions[i];

        int nameLength = 0;
        int size = 0;
        GLenum glType = GL_ZERO;

        glGetActiveUniform(
            program->handle,
            i,
            MAX_UNIFORM_NAME_SIZE-1,
            &nameLength,
            &size,
            &glType,
            def->name
        );
        def->name[MAX_UNIFORM_NAME_SIZE-1] = '\0';
        assert(nameLength > 0);

        def->location = glGetUniformLocation(program->handle, def->name);
        def->type = GLToUniformType(glType);
    }

    program->currentUniformValues = new UniformValue[count];
    memset(program->currentUniformValues, 0, sizeof(UniformValue)*count);
}

ShaderProgram* LinkShaderProgram( Shader** shaders, int shaderCount )
{
    for(int i = 0; i < shaderCount; i++)
    {
        if(!shaders[i] || shaders[i]->handle == INVALID_SHADER_HANDLE)
        {
            Error("Cannot link a shader program with invalid shaders.");
            return NULL;
        }
    }

    ShaderProgram* program = new ShaderProgram;
    memset(program, 0, sizeof(ShaderProgram));

    InitReferenceCounter(&program->refCounter);

    program->handle = glCreateProgram();
    GLuint programHandle = program->handle;

    program->shaderCount = shaderCount;
    program->shaders = new Shader*[shaderCount];
    memcpy(program->shaders, shaders, sizeof(Shader*)*shaderCount);

    for(int i = 0; i < shaderCount; i++)
    {
        ReferenceShader(shaders[i]);
        glAttachShader(programHandle, shaders[i]->handle);
    }

    BindVertexAttributes(programHandle);

    glLinkProgram(programHandle);
    {
        GLint state;
        glGetProgramiv(programHandle, GL_LINK_STATUS, &state);
        if(state)
        {
            Log("Linked shader program successfully");
            ShowShaderProgramLog(program, state);
        }
        else
        {
            Error("Error linking shader program");
            ShowShaderProgramLog(program, state);
            FreeShaderProgram(program);
            return NULL;
        }
    }

    glValidateProgram(programHandle);
    {
        GLint state;
        glGetProgramiv(programHandle, GL_VALIDATE_STATUS, &state);
        ShowShaderProgramLog(program, state);
        if(state)
        {
            Log("Validated shader program successfully");
            ShowShaderProgramLog(program, state);
        }
        else
        {
            Log("Error validating shader program");
            ShowShaderProgramLog(program, state);
        }
    }

    ReadUniformDefinitions(program);

    BindVertexAttributes(program->handle);

    return program;
}

static void FreeShaderProgram( ShaderProgram* program )
{
    FreeReferenceCounter(&program->refCounter);

    glDeleteProgram(program->handle);

    for(int i = 0; i < program->shaderCount; i++)
        ReleaseShader(program->shaders[i]);
    delete[] program->shaders;

    if(program->uniformDefinitions)
        delete[] program->uniformDefinitions;

    if(program->currentUniformValues)
        delete[] program->currentUniformValues;

    delete program;
}

void ReferenceShaderProgram( ShaderProgram* program )
{
    Reference(&program->refCounter);
}

void ReleaseShaderProgram( ShaderProgram* program )
{
    Release(&program->refCounter);
    if(!HasReferences(&program->refCounter))
        FreeShaderProgram(program);
}

static ShaderProgram* CurrentShaderProgram = NULL;

void BindShaderProgram( ShaderProgram* program )
{
    if(program != CurrentShaderProgram)
    {
        glUseProgram(program->handle);
        ApplyGlobalUniforms(program);
        CurrentShaderProgram = program;
    }
}

static int GetUniformIndex( const ShaderProgram* program, const char* name )
{
    for(int i = 0; i < program->uniformCount; i++)
        if(strncmp(name, program->uniformDefinitions[i].name, MAX_UNIFORM_NAME_SIZE-1) == 0)
            return i;
    return INVALID_UNIFORM_INDEX;
}

bool HasUniform( const ShaderProgram* program, const char* name )
{
    return GetUniformIndex(program, name) != INVALID_UNIFORM_INDEX;
}

static bool UniformValuesAreEqual( const UniformDefinition* definition,
                                   const UniformValue* a,
                                   const UniformValue* b )
{
    const int size = GetUniformSize(definition->type);
    return memcmp(a, b, size) == 0;
}

static void SetUniform( ShaderProgram* program, int index, const UniformValue* value )
{
    assert(program);
    assert(index >= 0);
    assert(index < program->uniformCount);
    assert(value);

    const UniformDefinition* def = &program->uniformDefinitions[index];
    const UniformValue* curValue = &program->currentUniformValues[index];
    if(!UniformValuesAreEqual(def, value, curValue))
    {
        BindShaderProgram(program);
        switch(def->type)
        {
            case INT_UNIFORM:
                glUniform1i(def->location, value->i);
                break;

            case FLOAT_UNIFORM:
                glUniform1f(def->location, value->f);
                break;

            case VEC3_UNIFORM:
                glUniform3fv(def->location, 1, (const float*)value);
                break;

            case VEC4_UNIFORM:
                glUniform4fv(def->location, 1, (const float*)value);
                break;

            case MAT3_UNIFORM:
                glUniformMatrix3fv(def->location, 1, GL_FALSE, (const float*)value);
                break;

            case MAT4_UNIFORM:
                glUniformMatrix4fv(def->location, 1, GL_FALSE, (const float*)value);
                break;
        }

        program->currentUniformValues[index] = *value;
    }
}

void SetUniform( ShaderProgram* program,
                 const char* name,
                 UniformType type,
                 const UniformValue* value )
{
    const int index = GetUniformIndex(program, name);
    if(index != INVALID_UNIFORM_INDEX)
        SetUniform(program, index, value);
}

static GlobalUniform* FindGlobalUniform( const char* name )
{
    for(int i = 0; i < MAX_GLOBAL_UNIFORMS; i++)
        if(strncmp(name, GlobalUniforms[i].name, MAX_UNIFORM_NAME_SIZE-1) == 0)
            return &GlobalUniforms[i];
    return NULL;
}

static GlobalUniform* FindFreeGlobalUniform()
{
    return FindGlobalUniform("");
}

void SetGlobalUniform( const char* name,
                       UniformType type,
                       const UniformValue* value )
{
    GlobalUniform* uniform = FindGlobalUniform(name);
    if(!uniform)
        uniform = FindFreeGlobalUniform();
    if(!uniform)
        FatalError("Too many global uniforms.");

    CopyString(name, uniform->name, sizeof(uniform->name));
    uniform->type = type;
    memcpy(&uniform->value, value, GetUniformSize(type));

    if(CurrentShaderProgram)
        SetUniform(CurrentShaderProgram,
                   uniform->name,
                   uniform->type,
                   &uniform->value);
}

void UnsetGlobalUniform( const char* name )
{
    GlobalUniform* uniform = FindGlobalUniform(name);
    if(uniform)
        memset(uniform, 0, sizeof(GlobalUniform));
}

/**
 * Sets all global uniforms in the given program.
 */
static void ApplyGlobalUniforms( ShaderProgram* program )
{
    for(int i = 0; i < MAX_GLOBAL_UNIFORMS; i++)
    {
        GlobalUniform* uniform = &GlobalUniforms[i];
        if(uniform->active)
            SetUniform(program,
                       uniform->name,
                       uniform->type,
                       &uniform->value);
    }
}


// ---- ShaderProgramSet ----

ShaderProgramSet* CreateShaderProgramSet( ShaderProgram* defaultProgram )
{
    ShaderProgramSet* set = new ShaderProgramSet;
    memset(set, 0, sizeof(ShaderProgramSet));
    InitReferenceCounter(&set->refCounter);
    SetShaderProgramFamily(set, "default", defaultProgram);
    return set;
}

static void FreeShaderProgramSet( ShaderProgramSet* set )
{
    FreeReferenceCounter(&set->refCounter);

    for(int i = 0; i < MAX_SHADER_PROGRAM_SET_ENTRIES; i++)
    {
        ShaderProgram* program = set->entries[i].program;
        if(program)
            ReleaseShaderProgram(program);
    }

    delete set;
}

void ReferenceShaderProgramSet( ShaderProgramSet* set )
{
    Reference(&set->refCounter);
}

void ReleaseShaderProgramSet( ShaderProgramSet* set )
{
    Release(&set->refCounter);
    if(!HasReferences(&set->refCounter))
        FreeShaderProgramSet(set);
}

static ShaderProgramSetEntry* FindShaderProgramSetEntry( ShaderProgramSet* set,
                                                         const char* family )
{
    for(int i = 0; i < MAX_SHADER_PROGRAM_SET_ENTRIES; i++)
    {
        ShaderProgramSetEntry* entry = &set->entries[i];
        if(strncmp(family, entry->family, MAX_PROGRAM_FAMILY_SIZE-1) == 0)
            return entry;
    }
    return NULL;
}

static const ShaderProgramSetEntry* FindShaderProgramSetEntry( const ShaderProgramSet* set,
                                                         const char* family )
{
    for(int i = 0; i < MAX_SHADER_PROGRAM_SET_ENTRIES; i++)
    {
        const ShaderProgramSetEntry* entry = &set->entries[i];
        if(strncmp(family, entry->family, MAX_PROGRAM_FAMILY_SIZE-1) == 0)
            return entry;
    }
    return NULL;
}

void SetShaderProgramFamily( ShaderProgramSet* set,
                             const char* family,
                             ShaderProgram* program )
{
    ShaderProgramSetEntry* entry = FindShaderProgramSetEntry(set, family);
    if(!entry)
        entry = FindShaderProgramSetEntry(set, "");

    if(entry)
    {
        CopyString(family, entry->family, sizeof(entry->family));

        if(entry->program)
            ReleaseShaderProgram(entry->program);
        entry->program = program;
        if(entry->program)
            ReferenceShaderProgram(entry->program);
    }
    else
    {
        FatalError("Too many family entries for shader program set %p.", set);
    }
}

ShaderProgram* GetShaderProgramByFamilyList( const ShaderProgramSet* set,
                                             const char* familyList )
{
    assert(strlen(familyList) <= MAX_PROGRAM_FAMILY_SIZE-1);

    char family[MAX_PROGRAM_FAMILY_SIZE];
    char* familyChar = family;
    const char* c = familyList;
    const ShaderProgramSetEntry* entry = NULL;

    while(true)
    {
        bool stop = false;
        switch(*c)
        {
            case '\0':
                stop = true;

            case ',':
                *familyChar = '\0';

                entry = FindShaderProgramSetEntry(set, family);
                if(entry)
                    return entry->program;

                familyChar = family;
                break;

            default:
                *familyChar = *c;
        }

        if(stop)
            break;

        familyChar++;
        c++;
    }

    entry = FindShaderProgramSetEntry(set, "default");
    if(entry)
        return entry->program;
    else
        return NULL;
}
