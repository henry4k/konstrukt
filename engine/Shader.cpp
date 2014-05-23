#include <assert.h>
#include <string.h> // strlen, memcmp

#include "Common.h"
#include "OpenGL.h"
#include "Vertex.h"
#include "Shader.h"


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

static void ReadUniformDescriptions( ShaderProgram* program )
{
    if(program->uniformDescriptions)
    {
        delete[] program->uniformDescriptions;
        program->uniformDescriptions = NULL;
    }

    if(program->defaultUniformValues)
    {
        delete[] program->defaultUniformValues;
        program->defaultUniformValues = NULL;
    }

    int count = 0;
    glGetProgramiv(program->handle, GL_ACTIVE_UNIFORMS, &count);

    program->uniformCount = count;

    program->uniformDescriptions = new UniformDescription[count];
    for(int i = 0; i < count; ++i)
    {
        UniformDescription* desc = program->uniformDescriptions[i];

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
            desc->name
        );
        assert(nameLength > 0);

        desc->location = glGetUniformLocation(program->handle, desc->name);
        desc->type = GLToUniformType(glType);
    }

    program->defaultUniformValues = new UniformValue[count];
    memset(program->defaultUniformValues, 0, sizeof(UniformValue)*count);
}


bool LinkShaderProgram( ShaderProgram* program, const Shader* shaders, int shaderCount )
{
    memset(program, 0, sizeof(ShaderProgram));

    for(int i = 0; i < shaderCount; i++)
    {
        if(shaders[i] == INVALID_SHADER)
        {
            Error("Cannot link a shader program with invalid shaders.");
            return false;
        }
    }

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
            return false;
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

    ReadUniformDescriptions(program);

    return true;
}

void FreeShaderProgram( ShaderProgram* program )
{
    glDeleteProgram(program->handle);
}

void BindShaderProgram( ShaderProgram* program )
{
    glUseProgram(program->handle);
}

int GetUniformIndex( ShaderProgram* program, const char* name )
{
    for(int i = 0; i < program->uniformCount; i++)
        if(strncmp(name, program->uniformDescriptions[i].name, MAX_UNIFORM_NAME_LENGTH) == 0)
            return i;
    return INVALID_UNIFORM_INDEX;
}

void SetUniformValueInShaderProgram( ShaderProgram* program, int index, UniformValue* value )
{
    assert(program);
    assert(index >= 0);
    assert(index < program->uniformCount);
    assert(value);

    BindShaderProgram(program->handle);
    const UniformDescription* desc = &program->uniformDescriptions[index];
    switch(desc->type)
    {
        case FLOAT_UNIFORM:
            glUniform1i(desc->location, value->f);
            break;

        case VEC3_UNIFORM:
            glUniform3fv(desc->location, 1, value->v3);
            break;

        case VEC4_UNIFORM:
            glUniform4fv(desc->location, 1, value->v4);
            break;

        case MAT3_UNIFORM:
            glUniformMatrix3fv(desc->location, 1, GL_FALSE, &(*value->m3)[0][0]);
            break;

        case MAT4_UNIFORM:
            glUniformMatrix4fv(desc->location, 1, GL_FALSE, &(*value->m4)[0][0]);
            break;
    }
}