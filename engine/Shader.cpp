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


// ----- Shader Object ------

static void ShowShaderObjectLog( ShaderObject shader )
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

static ShaderObject CreateShaderObject( const char* fileName, int type )
{
    ShaderObject shader = glCreateShader(type);

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
    ShowShaderObjectLog(shader);

    if(state)
    {
        Log("Compiled shader object successfully: %s", fileName);
    }
    else
    {
        Error("Error compiling shader object %s", fileName);
        return 0;
    }

    return shader;
}

ShaderObject LoadShaderObject( const char* fileName )
{
    if(StringEndsWith(fileName, ".vert"))
    {
        return CreateShaderObject(fileName, GL_VERTEX_SHADER);
    }
    else if(StringEndsWith(fileName, ".frag"))
    {
        return CreateShaderObject(fileName, GL_FRAGMENT_SHADER);
    }
    else
    {
        Error("Can't determine shader object type of file %s", fileName);
        return INVALID_SHADER_OBJECT;
    }
}

void FreeShaderObject( ShaderObject object )
{
    glDeleteShader(object);
}


// ----- Shader Program -----

void ShowShaderProgramLog( ShaderProgram program )
{
    GLint length = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);

    char* log = NULL;
    if(length > 1) // See ShowShaderProgramLog
    {
        log = new char[length];
        glGetProgramInfoLog(program, length, NULL, log);
    }

    if(log)
    {
        Log("%s", log);
        delete[] log;
    }
}

ShaderProgram LinkShaderProgram( const ShaderObject* objects, int objectCount )
{
    for(int i = 0; i < objectCount; i++)
    {
        if(objects[i] == INVALID_SHADER_OBJECT)
        {
            Error("Cannot link a shader program with invalid objects.");
            return INVALID_SHADER_PROGRAM;
        }
    }

    const ShaderProgram program = glCreateProgram();

    for(int i = 0; i < objectCount; i++)
        glAttachShader(program, objects[i]);

    BindVertexAttributes(program);

    glLinkProgram(program);
    {
        GLint state;
        glGetProgramiv(program, GL_LINK_STATUS, &state);
        ShowShaderProgramLog(program);

        if(state)
        {
            Log("Linked shader program successfully");
        }
        else
        {
            Error("Error linking shader programm");
            return INVALID_SHADER_PROGRAM;
        }
    }

    glValidateProgram(program);
    {
        GLint state;
        glGetProgramiv(program, GL_VALIDATE_STATUS, &state);
        ShowShaderProgramLog(program);
        if(state)
            Log("Validated shader program successfully");
        else
            Log("Error validating shader program");
    }

    return program;
}

void FreeShaderProgram( ShaderProgram program )
{
    glDeleteProgram(program);
}

void BindShaderProgram( ShaderProgram program )
{
    glUseProgram(program);
}


// ----- Uniform -----

typedef GLint UniformLocation;
static UniformLocation INVALID_UNIFORM_LOCATION = -1;

static UniformLocation GetUniformLocation( ShaderProgram program, const char* name )
{
    BindShaderProgram(program);
    const UniformLocation location = glGetUniformLocation(program, name);
    if(location >= 0)
        return location;
    glGetError(); // reset error (cause glGetUniformLocation sets an error value in this case)
    return INVALID_UNIFORM_LOCATION;
}

void SetUniform( ShaderProgram program, const char* name, int value )
{
    const UniformLocation location = GetUniformLocation(program, name);
    if(location != INVALID_UNIFORM_LOCATION)
        glUniform1i(location, value);
}

void SetUniform( ShaderProgram program, const char* name, float value )
{
    const UniformLocation location = GetUniformLocation(program, name);
    if(location != INVALID_UNIFORM_LOCATION)
        glUniform1f(location, value);
}

void SetUniform( ShaderProgram program, const char* name, int length, const float* value )
{
    const UniformLocation location = GetUniformLocation(program, name);
    if(location != INVALID_UNIFORM_LOCATION)
    {
        switch(length)
        {
            case 1: glUniform1fv(location, 1, value); break;
            case 2: glUniform2fv(location, 1, value); break;
            case 3: glUniform3fv(location, 1, value); break;
            case 4: glUniform4fv(location, 1, value); break;
        }
    }
}

void SetUniformMatrix3( ShaderProgram program, const char* name, const glm::mat3* value )
{
    const UniformLocation location = GetUniformLocation(program, name);
    if(location != INVALID_UNIFORM_LOCATION)
    {
        glUniformMatrix3fv(location, 1, GL_FALSE, &(*value)[0][0]);
    }
}

void SetUniformMatrix4( ShaderProgram program, const char* name, const glm::mat4* value )
{
    const UniformLocation location = GetUniformLocation(program, name);
    if(location != INVALID_UNIFORM_LOCATION)
    {
        glUniformMatrix4fv(location, 1, GL_FALSE, &(*value)[0][0]);
    }
}
