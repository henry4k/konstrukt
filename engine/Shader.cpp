#include <assert.h>
#include <string.h> // strlen, memcmp

#include "Common.h"
#include "OpenGL.h"
#include "Vertex.h"
#include "Shader.h"


struct UniformDefinition
{
    char name[MAX_UNIFORM_NAME_LENGTH];
    int location;
    UniformType type;
};

struct ShaderProgram
{
    GLuint handle;
    int uniformCount;
    UniformDefinition* uniformDefinitions;
    UniformValue* currentUniformValues;
    UniformValue* defaultUniformValues;
};


// ----- Tools ------

static char* LoadFile( const char* path, int* sizeOut )
{
    FILE* f = fopen(path, "r");
    if(!f)
    {
        Error("Can't open file %s", path);
        return 0;
    }

    fseek(f, 0, SEEK_END);
    int size = ftell(f);
    fseek(f, 0, SEEK_SET);

    char* b = new char[size];
    if(fread(b, 1, size, f) != size)
    {
        Error("Can't read file %s", path);
        delete[] b;
        fclose(f);
        return 0;
    }
    fclose(f);

    *sizeOut = size;
    return b;
}

static void FreeFile( const char* fileData )
{
    delete[] fileData;
}

bool StringEndsWith( const char* target, const char* end )
{
    const int targetLength = strlen(target);
    const int endLength = strlen(end);

    if(targetLength > endLength)
        return memcmp(&target[targetLength - endLength], end, endLength) == 0;
    else
        return false;
}


// ----- UniformValue ------

const float& UniformValue::f() const
{
    return *(const float*)data;
}

const glm::vec3& UniformValue::v3() const
{
    return *(const glm::vec3*)data;
}

const glm::vec4& UniformValue::v4() const
{
    return *(const glm::vec4*)data;
}

const glm::mat3& UniformValue::m3() const
{
    return *(const glm::mat3*)data;
}

const glm::mat4& UniformValue::m4() const
{
    return *(const glm::mat4*)data;
}

float& UniformValue::f()
{
    return *(float*)data;
}

glm::vec3& UniformValue::v3()
{
    return *(glm::vec3*)data;
}

glm::vec4& UniformValue::v4()
{
    return *(glm::vec4*)data;
}

glm::mat3& UniformValue::m3()
{
    return *(glm::mat3*)data;
}

glm::mat4& UniformValue::m4()
{
    return *(glm::mat4*)data;
}


// ----- Shader ------

static void ShowShaderLog( Shader shader )
{
    GLint length = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

    char* log = NULL;
    if(length > 1) // Some drivers wan't me to log single newline characters.
    {
        log = new char[length];
        glGetShaderInfoLog(shader, length, NULL, log);
    }

    if(log)
    {
        Log("%s", log);
        delete[] log;
    }
}

static Shader CreateShader( const char* fileName, int type )
{
    Shader shader = glCreateShader(type);

    int size;
    const char* source = LoadFile(fileName, &size);
    if(!source)
    {
        Error("Failed to read shader source %s", fileName);
        return 0;
    }

    glShaderSource(shader, 1, &source, &size);
    FreeFile(source);

    glCompileShader(shader);

    GLint state;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &state);
    ShowShaderLog(shader);

    if(state)
    {
        Log("Compiled shader successfully: %s", fileName);
    }
    else
    {
        Error("Error compiling shader %s", fileName);
        return 0;
    }

    return shader;
}

Shader LoadShader( const char* fileName )
{
    if(StringEndsWith(fileName, ".vert"))
    {
        return CreateShader(fileName, GL_VERTEX_SHADER);
    }
    else if(StringEndsWith(fileName, ".frag"))
    {
        return CreateShader(fileName, GL_FRAGMENT_SHADER);
    }
    else
    {
        Error("Can't determine shader type of file %s", fileName);
        return INVALID_SHADER;
    }
}

void FreeShader( Shader shader )
{
    glDeleteShader(shader);
}


// ----- Shader Program -----

void ShowShaderProgramLog( ShaderProgram* program )
{
    GLint length = 0;
    glGetProgramiv(program->handle, GL_INFO_LOG_LENGTH, &length);

    char* log = NULL;
    if(length > 1) // See ShowShaderProgramLog
    {
        log = new char[length];
        glGetProgramInfoLog(program->handle, length, NULL, log);
    }

    if(log)
    {
        Log("%s", log);
        delete[] log;
    }
}

static UniformType GLToUniformType( GLenum glType )
{
    switch(glType)
    {
        case GL_FLOAT: return FLOAT_UNIFORM;
        case GL_FLOAT_VEC3: return VEC3_UNIFORM;
        case GL_FLOAT_VEC4: return VEC4_UNIFORM;
        case GL_FLOAT_MAT3: return MAT3_UNIFORM;
        case GL_FLOAT_MAT4: return MAT4_UNIFORM;

        default:
            FatalError("Unsupported gl constant!");
            return FLOAT_UNIFORM;
    }
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

    if(program->defaultUniformValues)
    {
        delete[] program->defaultUniformValues;
        program->defaultUniformValues = NULL;
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
            MAX_UNIFORM_NAME_LENGTH,
            &nameLength,
            &size,
            &glType,
            def->name
        );
        assert(nameLength > 0);

        def->location = glGetUniformLocation(program->handle, def->name);
        def->type = GLToUniformType(glType);
    }

    program->currentUniformValues = new UniformValue[count];
    memset(program->currentUniformValues, 0, sizeof(UniformValue)*count);

    program->defaultUniformValues = new UniformValue[count];
    memset(program->defaultUniformValues, 0, sizeof(UniformValue)*count);
}

ShaderProgram* LinkShaderProgram( const Shader* shaders, int shaderCount )
{
    for(int i = 0; i < shaderCount; i++)
    {
        if(shaders[i] == INVALID_SHADER)
        {
            Error("Cannot link a shader program with invalid shaders.");
            return NULL;
        }
    }

    ShaderProgram* program = new ShaderProgram;
    memset(program, 0, sizeof(ShaderProgram));

    program->handle = glCreateProgram();
    GLuint programHandle = program->handle;

    for(int i = 0; i < shaderCount; i++)
        glAttachShader(programHandle, shaders[i]);

    BindVertexAttributes(programHandle);

    glLinkProgram(programHandle);
    {
        GLint state;
        glGetProgramiv(programHandle, GL_LINK_STATUS, &state);
        ShowShaderProgramLog(program);

        if(state)
        {
            Log("Linked shader program successfully");
        }
        else
        {
            Error("Error linking shader programm");
            delete program;
            return NULL;
        }
    }

    glValidateProgram(programHandle);
    {
        GLint state;
        glGetProgramiv(programHandle, GL_VALIDATE_STATUS, &state);
        ShowShaderProgramLog(program);
        if(state)
            Log("Validated shader program successfully");
        else
            Log("Error validating shader program");
    }

    ReadUniformDefinitions(program);

    BindVertexAttributes(program->handle);

    return program;
}

void FreeShaderProgram( ShaderProgram* program )
{
    glDeleteProgram(program->handle);

    if(program->uniformDefinitions)
        delete[] program->uniformDefinitions;

    if(program->currentUniformValues)
        delete[] program->currentUniformValues;

    if(program->defaultUniformValues)
        delete[] program->defaultUniformValues;

    delete program;
}

void BindShaderProgram( const ShaderProgram* program )
{
    glUseProgram(program->handle);
}

int GetUniformCount( const ShaderProgram* program )
{
    return program->uniformCount;
}

int GetUniformIndex( const ShaderProgram* program, const char* name )
{
    for(int i = 0; i < program->uniformCount; i++)
        if(strncmp(name, program->uniformDefinitions[i].name, MAX_UNIFORM_NAME_LENGTH) == 0)
            return i;
    return INVALID_UNIFORM_INDEX;
}

void SetUniform( ShaderProgram* program, int index, const UniformValue* value )
{
    assert(program);
    assert(index >= 0);
    assert(index < program->uniformCount);
    assert(value);

    // TODO: Check currentUniformValues here!

    BindShaderProgram(program);
    const UniformDefinition* def = &program->uniformDefinitions[index];
    switch(def->type)
    {
        case FLOAT_UNIFORM:
            glUniform1i(def->location, value->f());
            break;

        case VEC3_UNIFORM:
            glUniform3fv(def->location, 1, value->data);
            break;

        case VEC4_UNIFORM:
            glUniform4fv(def->location, 1, value->data);
            break;

        case MAT3_UNIFORM:
            glUniformMatrix3fv(def->location, 1, GL_FALSE, value->data);
            break;

        case MAT4_UNIFORM:
            glUniformMatrix4fv(def->location, 1, GL_FALSE, value->data);
            break;
    }
}

void SetUniformDefault( ShaderProgram* program, int index, const UniformValue* value )
{
    assert(index >= 0);
    assert(index < program->uniformCount);

    program->defaultUniformValues[index] = *value;
}

void ResetUniform( ShaderProgram* program, int index )
{
    assert(index >= 0);
    assert(index < program->uniformCount);

    const UniformValue* defaultValue = &program->defaultUniformValues[index];
    SetUniform(program, index, defaultValue);
}

void SetUniform( ShaderProgram* program, const char* name, const UniformValue* value )
{
    const int index = GetUniformIndex(program, name);
    if(index != INVALID_UNIFORM_INDEX)
        SetUniform(program, index, value);
}

void SetUniformDefault( ShaderProgram* program, const char* name, const UniformValue* value )
{
    const int index = GetUniformIndex(program, name);
    if(index != INVALID_UNIFORM_INDEX)
        SetUniformDefault(program, index, value);
}

void ResetUniform( ShaderProgram* program, const char* name )
{
    const int index = GetUniformIndex(program, name);
    if(index != INVALID_UNIFORM_INDEX)
        ResetUniform(program, index);
}
