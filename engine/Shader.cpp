#include <assert.h>
#include <string.h> // strncmp, strlen, strncpy, memcmp, memset, memcpy
#include <stdlib.h> // qsort
#include <stdint.h> // uintptr_t

#include "Common.h"
#include "Crc32.h"
#include "Texture.h"
#include "Vfs.h"
#include "OpenGL.h"
#include "Vertex.h"
#include "Reference.h"
#include "Shader.h"


static const GLuint INVALID_SHADER_HANDLE = 0;
static const int MAX_GLOBAL_UNIFORMS = 32;
static const int MAX_SHADER_PROGRAM_SET_ENTRIES = 16;

enum ShaderVariableType
{
    UNIFORM_VARIABLE,
    TEXTURE_VARIABLE,
    UNIFORM_BUFFER_VARIABLE
};


struct Shader
{
    ReferenceCounter refCounter;
    GLuint handle;
};

struct UniformDefinition
{
    uint32_t nameHash;
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

    ShaderVariableSet* variableSet;
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

struct UniformBuffer
{
    ReferenceCounter refCounter;

    GLuint handle;

    // TODO: layout info goes here

    int size;
    char* data;

    bool dirty;
};

struct ShaderVariable
{
    uint32_t nameHash;
    char name[MAX_UNIFORM_NAME_SIZE];

    ShaderVariableType type;
    union
    {
        union
        {
            UniformType  type;
            UniformValue value;
        } uniform;
        Texture*       texture;
        UniformBuffer* uniformBuffer;
    } value;
};

struct ShaderVariableSet
{
    ShaderVariable entries[MAX_SHADER_VARIABLE_SET_ENTRIES];
};


static ShaderVariableSet* GlobalShaderVariableSet = NULL;


void InitShader()
{
    GlobalShaderVariableSet = CreateShaderVariableSet();
}

void DestroyShader()
{
    FreeShaderVariableSet(GlobalShaderVariableSet);
}

ShaderVariableSet* GetGlobalShaderVariableSet()
{
    return GlobalShaderVariableSet;
}


// ----- Tools ------

static bool StringEndsWith( const char* target, const char* end )
{
    const int targetLength = strlen(target);
    const int endLength = strlen(end);

    if(targetLength > endLength)
        return memcmp(&target[targetLength - endLength], end, endLength) == 0;
    else
        return false;
}


// ----- Shader ------

static void FreeShader( Shader* shader );

static void ShowShaderLog( Shader* shader, bool success, const char* vfsPath )
{
    GLint length = 0;
    glGetShaderiv(shader->handle, GL_INFO_LOG_LENGTH, &length);

    char* log = NULL;
    if(length > 1) // Some drivers want me to log single newline characters.
    {
        log = new char[length];
        glGetShaderInfoLog(shader->handle, length, NULL, log);
    }

    if(success)
        Warn("Compiled shader: %s\n%s", vfsPath, log);
    else
        FatalError("Error compiling shader %s\n%s", vfsPath, log);

    if(log)
        delete[] log;
}

static Shader* CreateShader( const char* vfsPath, int type )
{
    VfsFile* file = OpenVfsFile(vfsPath, VFS_OPEN_READ);
    if(!file)
        return NULL;

    const int fileSize = GetVfsFileSize(file);
    char* fileContent = (char*)Alloc(fileSize);
    ReadVfsFile(file, fileContent, fileSize);
    CloseVfsFile(file);

    Shader* shader = new Shader;
    InitReferenceCounter(&shader->refCounter);
    shader->handle = glCreateShader(type);

    glShaderSource(shader->handle, 1, &fileContent, &fileSize);
    Free(fileContent);

    glCompileShader(shader->handle);

    GLint state;
    glGetShaderiv(shader->handle, GL_COMPILE_STATUS, &state);
    ShowShaderLog(shader, state, vfsPath);
    return shader;
}

Shader* LoadShader( const char* vfsPath )
{
    if(StringEndsWith(vfsPath, ".vert"))
    {
        return CreateShader(vfsPath, GL_VERTEX_SHADER);
    }
    else if(StringEndsWith(vfsPath, ".frag"))
    {
        return CreateShader(vfsPath, GL_FRAGMENT_SHADER);
    }
    else
    {
        FatalError("Can't determine shader type of file %s", vfsPath);
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

static void ShowShaderProgramLog( ShaderProgram* program, bool success, const char* message )
{
    GLint length = 0;
    glGetProgramiv(program->handle, GL_INFO_LOG_LENGTH, &length);

    char* log = NULL;
    if(length > 1) // See ShowShaderLog
    {
        log = new char[length];
        glGetProgramInfoLog(program->handle, length, NULL, log);
    }

    if(success)
        Warn("%s\n%s", message, log);
    else
        FatalError("%s\n%s", message, log);

    if(log)
        delete[] log;
}

static UniformType GLToUniformType( GLenum glType )
{
    switch(glType)
    {
        case GL_INT:          return INT_UNIFORM;
        case GL_SAMPLER_1D:
        case GL_SAMPLER_2D:
        case GL_SAMPLER_3D:
        case GL_SAMPLER_CUBE: return SAMPLER_UNIFORM;
        case GL_FLOAT:        return FLOAT_UNIFORM;
        case GL_FLOAT_VEC3:   return VEC3_UNIFORM;
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
        case SAMPLER_UNIFORM:
        case INT_UNIFORM:     return sizeof(int);
        case FLOAT_UNIFORM:   return sizeof(float);
        case VEC3_UNIFORM:    return sizeof(Vec3);
        case MAT3_UNIFORM:    return sizeof(Mat3);
        case MAT4_UNIFORM:    return sizeof(Mat4);
    }
    FatalError("Unknown UniformType!");
    return 0;
}

static int CountUniforms( const ShaderProgram* program )
{
    static char name[MAX_UNIFORM_NAME_SIZE];

    int count = 0;
    int activeUniformCount = 0;
    glGetProgramiv(program->handle, GL_ACTIVE_UNIFORMS, &activeUniformCount);
    REPEAT(activeUniformCount, i)
    {
        int size = 0;
        GLenum glType = GL_ZERO;

        glGetActiveUniform(
            program->handle,
            i,
            MAX_UNIFORM_NAME_SIZE-1,
            NULL,
            &size,
            &glType,
            name
        );

        const int location = glGetUniformLocation(program->handle, name);
        if(location == -1)
            continue;

        count += size; // treat array elements as full uniforms
    }
    return count;
}

static void AddUniformDefinition( ShaderProgram* program,
                                  int* index,
                                  const char* name,
                                  int location,
                                  UniformType type )
{
    assert(*index < program->uniformCount);
    UniformDefinition* def = &program->uniformDefinitions[*index];

    const int nameLength = strlen(name);
    assert(nameLength <= MAX_UNIFORM_NAME_SIZE-1);

    def->nameHash = CalcCrc32ForString(name);
    memset(def->name, 0, MAX_UNIFORM_NAME_SIZE);
    strncpy(def->name, name, nameLength);
    def->location = location;
    def->type     = type;

    (*index)++;
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

    int count = CountUniforms(program);
    program->uniformCount = count;
    program->uniformDefinitions = new UniformDefinition[count];

    // See CountUniforms:
    int index = 0;
    static char name[MAX_UNIFORM_NAME_SIZE];
    int activeUniformCount = 0;
    glGetProgramiv(program->handle, GL_ACTIVE_UNIFORMS, &activeUniformCount);
    for(int i = 0; i < activeUniformCount; ++i)
    {
        int nameLength = 0;
        int size = 0;
        GLenum glType = GL_ZERO;

        glGetActiveUniform(
            program->handle,
            i,
            MAX_UNIFORM_NAME_SIZE,
            &nameLength,
            &size,
            &glType,
            name
        );

        assert(nameLength > 0);
        assert(nameLength <= MAX_UNIFORM_NAME_SIZE-1);

        const int location = glGetUniformLocation(program->handle, name);
        const UniformType type = GLToUniformType(glType);

        if(location == -1)
            continue;

        assert(size >= 1);
        if(size > 1) // array:
        {
            assert(StringEndsWith(name, "[0]"));
            name[nameLength-3] = '\0';

            for(int j = 0; j < size; j++)
            {
                const char* elementName = Format("%s[%d]", name, j);
                AddUniformDefinition(program, &index, elementName, location+j, type);
            }
        }
        else
        {
            AddUniformDefinition(program, &index, name, location, type);
        }
    }

    program->currentUniformValues = new UniformValue[count];
    memset(program->currentUniformValues, 0, sizeof(UniformValue)*count);
}

static void ReadUniformBlockDefinitions( ShaderProgram* program )
{
    static char name[MAX_UNIFORM_NAME_SIZE];

    int blockCount = 0;
    glGetProgramiv(program->handle, GL_ACTIVE_UNIFORM_BLOCKS, &blockCount);
    REPEAT(blockCount, i)
    {
        int nameLength = 0;
        int size = 0;
        int uniformCount = 0;
        glGetActiveUniformBlockName(program->handle,
                                    i,
                                    MAX_UNIFORM_NAME_SIZE,
                                    &nameLength,
                                    name);
        glGetActiveUniformBlockiv(program->handle,
                                  i,
                                  GL_UNIFORM_BLOCK_DATA_SIZE,
                                  &size);
        glGetActiveUniformBlockiv(program->handle,
                                  i,
                                  GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS,
                                  &uniformCount);
        Log("BLOCK: %d %s size=%d uniforms=%d", i, name, size, uniformCount);

        int* uniformIndices_ = new int[uniformCount];
        GLuint* uniformIndices = new GLuint[uniformCount];
        glGetActiveUniformBlockiv(program->handle,
                                  i,
                                  GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES,
                                  uniformIndices_);
        REPEAT(uniformCount, j)
            uniformIndices[j] = uniformIndices_[j];
        delete[] uniformIndices_;
        int* uniformOffset = new int[uniformCount];
        int* uniformArrStride = new int[uniformCount];
        int* uniformMatStride = new int[uniformCount];
        glGetActiveUniformsiv(program->handle, uniformCount, uniformIndices, GL_UNIFORM_OFFSET, uniformOffset);
        glGetActiveUniformsiv(program->handle, uniformCount, uniformIndices, GL_UNIFORM_ARRAY_STRIDE, uniformArrStride);
        glGetActiveUniformsiv(program->handle, uniformCount, uniformIndices, GL_UNIFORM_MATRIX_STRIDE, uniformMatStride);
        REPEAT(uniformCount, j)
            Log("  %d location=%d offset=%d arrStride=%d matStride=%d",
                j, uniformIndices[j], uniformOffset[j], uniformArrStride[j], uniformMatStride[j]);
        delete[] uniformIndices;
        delete[] uniformOffset;
        delete[] uniformArrStride;
        delete[] uniformMatStride;

        // uniformOffset:
        // offset, in basic machine units, relative to the beginning of the uniform block
        //
        // uniformArrStride:
        // difference, in basic machine units, of consecutive elements in an array
        //
        // uniformMatStride:
        // stride between columns of a column-major matrix or rows of a row-major matrix, in basic machine units
    }
}

ShaderProgram* LinkShaderProgram( Shader** shaders, int shaderCount )
{
    for(int i = 0; i < shaderCount; i++)
        if(!shaders[i] || shaders[i]->handle == INVALID_SHADER_HANDLE)
            FatalError("Cannot link a shader program with invalid shaders.");

    ShaderProgram* program = new ShaderProgram;
    memset(program, 0, sizeof(ShaderProgram));

    InitReferenceCounter(&program->refCounter);

    program->handle = glCreateProgram();
    GLuint programHandle = program->handle;

    program->shaderCount = shaderCount;
    program->shaders = new Shader*[shaderCount];
    memcpy(program->shaders, shaders, sizeof(Shader*)*shaderCount);
    program->variableSet = CreateShaderVariableSet();

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
            ShowShaderProgramLog(program, state, "Linked shader program successfully");
        }
        else
        {
            ShowShaderProgramLog(program, state, "Error linking shader program");
            FreeShaderProgram(program);
            return NULL;
        }
    }

    glValidateProgram(programHandle);
    {
        GLint state;
        glGetProgramiv(programHandle, GL_VALIDATE_STATUS, &state);
        if(state)
        {
            ShowShaderProgramLog(program, state, "Validated shader program successfully");
        }
        else
        {
            ShowShaderProgramLog(program, state, "Error validating shader program");
        }
    }

    ReadUniformDefinitions(program);
    ReadUniformBlockDefinitions(program);

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

    FreeShaderVariableSet(program->variableSet);

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
        CurrentShaderProgram = program;
    }
}

ShaderVariableSet* GetShaderProgramShaderVariableSet( const ShaderProgram* program )
{
    return program->variableSet;
}

static int GetUniformIndexByNameHash( const ShaderProgram* program, uint32_t nameHash )
{
    for(int i = 0; i < program->uniformCount; i++)
        if(program->uniformDefinitions[i].nameHash == nameHash)
            return i;
    return INVALID_UNIFORM_INDEX;
}

static int GetUniformIndexByName( const ShaderProgram* program, const char* name )
{
    return GetUniformIndexByNameHash(program, CalcCrc32ForString(name));
}

bool HasUniform( const ShaderProgram* program, const char* name )
{
    return GetUniformIndexByName(program, name) != INVALID_UNIFORM_INDEX;
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
    assert(index >= 0);
    assert(index < program->uniformCount);

    const UniformDefinition* def = &program->uniformDefinitions[index];
    const UniformValue* curValue = &program->currentUniformValues[index];
    if(!UniformValuesAreEqual(def, value, curValue))
    {
        BindShaderProgram(program);
        switch(def->type)
        {
            case INT_UNIFORM:
            case SAMPLER_UNIFORM:
                glUniform1iv(def->location, 1, (const int*)value);
                break;

            case FLOAT_UNIFORM:
                glUniform1fv(def->location, 1, (const float*)value);
                break;

            case VEC3_UNIFORM:
                glUniform3fv(def->location, 1, (const float*)value);
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
    if(!entry)
        FatalError("Too many family entries for shader program set %p.", set);

    CopyString(family, entry->family, sizeof(entry->family));

    if(entry->program)
        ReleaseShaderProgram(entry->program);
    entry->program = program;
    if(entry->program)
        ReferenceShaderProgram(entry->program);
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

// ---- UniformBuffer ----

UniformBuffer* CreateUniformBuffer()
{
    return NULL; // TODO
}

void ReferenceUniformBuffer( UniformBuffer* buffer )
{
    // TODO
}

void ReleaseUniformBuffer( UniformBuffer* buffer )
{
    // TODO
}



// ---- ShaderVariableSet ----

ShaderVariableSet* CreateShaderVariableSet()
{
    ShaderVariableSet* set = new ShaderVariableSet;
    memset(set, 0, sizeof(ShaderVariableSet));
    return set;
}

void FreeShaderVariableSet( ShaderVariableSet* set )
{
    ClearShaderVariableSet(set);
    delete set;
}

static void FreeShaderVariable( ShaderVariable* var )
{
    //var->nameHash = 0;
    switch(var->type)
    {
        case UNIFORM_VARIABLE:
            break;

        case TEXTURE_VARIABLE:
            if(var->value.texture)
                ReleaseTexture(var->value.texture);
            break;

        case UNIFORM_BUFFER_VARIABLE:
            if(var->value.uniformBuffer)
                ReleaseUniformBuffer(var->value.uniformBuffer);
            break;
    }

    // TODO: Shouldn't be needed:
    memset(var, 0, sizeof(ShaderVariable));
}

void ClearShaderVariableSet( ShaderVariableSet* set )
{
    REPEAT(MAX_SHADER_VARIABLE_SET_ENTRIES, i)
        FreeShaderVariable(&set->entries[i]);
}

static ShaderVariable* FindShaderVariableByNameHash( ShaderVariableSet* set,
                                                     uint32_t nameHash )
{
    REPEAT(MAX_SHADER_VARIABLE_SET_ENTRIES, i)
        if(set->entries[i].nameHash == nameHash)
            return &set->entries[i];
    return NULL;
}

static const ShaderVariable* FindConstShaderVariableByNameHash( const ShaderVariableSet* set,
                                                                uint32_t nameHash )
{
    REPEAT(MAX_SHADER_VARIABLE_SET_ENTRIES, i)
        if(set->entries[i].nameHash == nameHash)
            return &set->entries[i];
    return NULL;
}

static const ShaderVariable* FindConstShaderVariableInSetsByNameHash( const ShaderVariableSet** set,
                                                                      int setCount,
                                                                      uint32_t nameHash )
{
    REPEAT(setCount, i)
    {
        const ShaderVariable* var = FindConstShaderVariableByNameHash(set[i], nameHash);
        if(var)
            return var;
    }
    return NULL;
}

static ShaderVariable* PrepareNewShaderVariable( ShaderVariableSet* set,
                                                 const char* name,
                                                 ShaderVariableType type )
{
    uint32_t nameHash = CalcCrc32ForString(name);
    ShaderVariable* var = FindShaderVariableByNameHash(set, nameHash);
    if(!var)
    {
        var = FindShaderVariableByNameHash(set, 0);
        if(!var)
            FatalError("Too many entries for shader variable set %p.", set);
    }
    FreeShaderVariable(var);

    var->nameHash = nameHash;
    CopyString(name, var->name, sizeof(var->name));
    var->type = type;

    return var;
}

static void SetUniformVariable( ShaderVariableSet* set,
                                const char* name,
                                UniformType type,
                                const void* value )
{
    ShaderVariable* var = PrepareNewShaderVariable(set, name, UNIFORM_VARIABLE);
    var->value.uniform.type = type;
    memcpy(&var->value.uniform.value, value, GetUniformSize(type));
}

void SetIntUniform( ShaderVariableSet* set, const char* name, int value )
{
    SetUniformVariable(set, name, INT_UNIFORM, &value);
}

void SetFloatUniform( ShaderVariableSet* set, const char* name, float value )
{
    SetUniformVariable(set, name, FLOAT_UNIFORM, &value);
}

void SetVec3Uniform( ShaderVariableSet* set, const char* name, Vec3 value )
{
    SetUniformVariable(set, name, VEC3_UNIFORM, &value);
}

void SetMat3Uniform( ShaderVariableSet* set, const char* name, Mat3 value )
{
    SetUniformVariable(set, name, MAT3_UNIFORM, &value);
}

void SetMat4Uniform( ShaderVariableSet* set, const char* name, Mat4 value )
{
    SetUniformVariable(set, name, MAT4_UNIFORM, &value);
}

void SetTexture( ShaderVariableSet* set, const char* name, Texture* texture )
{
    ShaderVariable* var = PrepareNewShaderVariable(set, name, TEXTURE_VARIABLE);
    ReferenceTexture(texture);
    var->value.texture = texture;
}

void SetUniformBuffer( ShaderVariableSet* set, const char* name, UniformBuffer* buffer )
{
    ShaderVariable* var = PrepareNewShaderVariable(set, name, UNIFORM_BUFFER_VARIABLE);
    ReferenceUniformBuffer(buffer);
    var->value.uniformBuffer = buffer;
}

void UnsetShaderVariable( ShaderVariableSet* set, const char* name )
{
    uint32_t nameHash = CalcCrc32ForString(name);
    ShaderVariable* entry = FindShaderVariableByNameHash(set, nameHash);
    if(entry)
        FreeShaderVariable(entry);
}

void CopyShaderVariablesAsArrayElements( ShaderVariableSet* destinationSet,
                                         const ShaderVariableSet* sourceSet,
                                         int arrayIndex )
{
    REPEAT(MAX_SHADER_VARIABLE_SET_ENTRIES, i)
    {
        const ShaderVariable* sourceVar = &sourceSet->entries[i];
        if(sourceVar->nameHash)
        {
            const char* newName = Format("%s[%d]", sourceVar->name, arrayIndex);
            ShaderVariable* destinationVar =
                PrepareNewShaderVariable(destinationSet, newName, sourceVar->type);
            destinationVar->value = sourceVar->value;
        }
    }
}

static void AddTextureBinding( ShaderVariableBindings* bindings,
                               Texture* texture )
{
    // Don't add a texture twice
    REPEAT(bindings->textureCount, i)
        if(bindings->textures[i] == texture)
            return;

    if(bindings->textureCount >= MAX_TEXTURE_UNITS)
        FatalError("Can\'t bind any more textures!");

    bindings->textures[bindings->textureCount] = texture;
    bindings->textureCount++;
}

static int Compare( uintptr_t a, uintptr_t b )
{
    if(a == b)
        return 0;
    else if(a < b)
        return -1;
    else
        return 1;
}

static int CompareTextures( const void* a_, const void* b_ )
{
    const Texture* a = (const Texture*)a_;
    const Texture* b = (const Texture*)b_;
    return Compare((uintptr_t)a,
                   (uintptr_t)b);
}

void GatherShaderVariableBindings( const ShaderProgram* program,
                                   ShaderVariableBindings* bindings,
                                   const ShaderVariableSet** variableSets,
                                   int variableSetCount )
{
    memset(bindings, 0, sizeof(ShaderVariableBindings));
    REPEAT(program->uniformCount, i)
    {
        const UniformDefinition* definition = &program->uniformDefinitions[i];
        if(definition->type == SAMPLER_UNIFORM)
        {
            const ShaderVariable* var =
                FindConstShaderVariableInSetsByNameHash(variableSets,
                                                        variableSetCount,
                                                        definition->nameHash);
            if(!var)
            //    FatalError("Can\'t bind uniform %s:  Not available in any ShaderVariableSet.", definition->name);

            assert(var->type == TEXTURE_VARIABLE);
            AddTextureBinding(bindings,
                                var->value.texture);
        }
    }

    // Sort texture bindings:
    qsort(bindings->textures,
          bindings->textureCount,
          sizeof(Texture*),
          CompareTextures);
}

static int GetTextureUnit( const ShaderVariableBindings* bindings,
                           const ShaderVariable* var )
{
    REPEAT(bindings->textureCount, i)
        if(bindings->textures[i] == var->value.texture)
            return i;
    FatalError("No binding was generated for texture %p from uniform %s.",
               var->value.texture,
               var->name);
    return 0;
}

void SetShaderProgramUniforms( ShaderProgram* program,
                               const ShaderVariableSet** variableSets,
                               int variableSetCount,
                               const ShaderVariableBindings* bindings )
{
    REPEAT(program->uniformCount, i)
    {
        const UniformDefinition* definition = &program->uniformDefinitions[i];
        const ShaderVariable* var =
            FindConstShaderVariableInSetsByNameHash(variableSets,
                                                    variableSetCount,
                                                    definition->nameHash);
        if(!var)
        //    FatalError("Can\'t set uniform %s:  Not available in any ShaderVariableSet.", definition->name);

        switch(var->type)
        {
            case UNIFORM_VARIABLE:
                SetUniform(program, i, &var->value.uniform.value);
                break;

            case TEXTURE_VARIABLE:
                UniformValue v;
                v.i = GetTextureUnit(bindings, var);
                SetUniform(program, i, &v);
                break;

            case UNIFORM_BUFFER_VARIABLE:
                FatalError("Uniform blocks are no uniforms.");
        }
    }
}
