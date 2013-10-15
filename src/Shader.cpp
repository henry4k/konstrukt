#include "Common.h"
#include "OpenGL.h"
#include "Vertex.h"
#include "Shader.h"

bool GetUniformLocation( GLuint shader, const char* name, int* location )
{
    BindShader(shader);
    *location = glGetUniformLocation(shader, name);
    if(*location >= 0)
        return true;
    glGetError(); // reset error (cause glGetUniformLocation sets an error value in this case <.<)
    return false;
}

void SetUniform( GLuint shader, const char* name, int value )
{
    int loc;
    if(GetUniformLocation(shader,name,&loc))
        glUniform1i(loc, value);
}

void SetUniform( GLuint shader, const char* name, float value )
{
    int loc;
    if(GetUniformLocation(shader,name,&loc))
        glUniform1f(loc, value);
}

void SetUniform( GLuint shader, const char* name, int length, float* value )
{
    int loc;
    if(GetUniformLocation(shader,name,&loc))
    {
        switch(length)
        {
            case 1: glUniform1fv(loc, 1, value); break;
            case 2: glUniform2fv(loc, 1, value); break;
            case 3: glUniform3fv(loc, 1, value); break;
            case 4: glUniform4fv(loc, 1, value); break;
        }
    }
}

char* LoadFile( const char* path, int* sizeOut )
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

void FreeFile( const char* fileData )
{
    delete[] fileData;
}

void ShowShaderLog( GLuint handle )
{
    GLint length = 0;
    glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &length);

    char* log = NULL;
    if(length > 1) // Some drivers wan't me to log single newline characters.
    {
        log = new char[length];
        glGetShaderInfoLog(handle, length, NULL, log);
    }

    if(log)
    {
        Log("%s", log);
        delete[] log;
    }
}

void ShowProgramLog( GLuint handle )
{
    GLint length = 0;
    glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &length);

    char* log = NULL;
    if(length > 1) // See ShowShaderLog
    {
        log = new char[length];
        glGetProgramInfoLog(handle, length, NULL, log);
    }

    if(log)
    {
        Log("%s", log);
        delete[] log;
    }
}

GLuint CreateShaderObject( const char* file, int type )
{
    GLuint handle = glCreateShader(type);

    int size;
    const char* source = LoadFile(file, &size);
    if(!source)
    {
        Error("Failed to read shader source %s", file);
        return 0;
    }

    glShaderSource(handle, 1, &source, &size);
    FreeFile(source);

    glCompileShader(handle);


    GLint state;
    glGetShaderiv(handle, GL_COMPILE_STATUS, &state);
    ShowShaderLog(handle);

    if(state)
    {
        Log("Compiled shader object successfully: %s", file);
    }
    else
    {
        Error("Error compiling shader object %s", file);
        return 0;
    }

    return handle;
}

GLuint LoadShader( const char* vert, const char* frag )
{
    GLuint vertObject = CreateShaderObject(vert, GL_VERTEX_SHADER);
    if(!vertObject)
        return 0;

    GLuint fragObject = CreateShaderObject(frag, GL_FRAGMENT_SHADER);
    if(!fragObject)
        return 0;

    const GLuint program = glCreateProgram();
    glAttachShader(program, vertObject);
    glAttachShader(program, fragObject);
    BindVertexAttributes(program);

    glLinkProgram(program);
    {
        GLint state;
        glGetProgramiv(program, GL_LINK_STATUS, &state);
        ShowProgramLog(program);

        if(state)
        {
            Log("Linked shader program successfully (%s, %s)", vert, frag);
        }
        else
        {
            Error("Error linking shader programm (%s, %s)", vert, frag);
            return 0;
        }
    }

    glValidateProgram(program);
    {
        GLint state;
        glGetProgramiv(program, GL_VALIDATE_STATUS, &state);
        ShowProgramLog(program);
        if(state)
            Log("Validated shader program successfully");
        else
            Log("Error validating shader program");
    }

    return program;
}

void BindShader( GLuint handle )
{
    glUseProgram(handle);
}

void FreeShader( GLuint handle )
{
    glDeleteProgram(handle);
}
