#include "Common.h"
#include "OpenGL.h"
#include "Vertex.h"
#include "Shader.h"

typedef GLuint Shader;

bool GetUniformLocation( Program program, const char* name, int* location )
{
    BindProgram(program);
    *location = glGetUniformLocation(program, name);
    if(*location >= 0)
        return true;
    glGetError(); // reset error (cause glGetUniformLocation sets an error value in this case <.<)
    return false;
}

void SetUniform( Program program, const char* name, int value )
{
    int loc;
    if(GetUniformLocation(program,name,&loc))
        glUniform1i(loc, value);
}

void SetUniform( Program program, const char* name, float value )
{
    int loc;
    if(GetUniformLocation(program,name,&loc))
        glUniform1f(loc, value);
}

void SetUniform( Program program, const char* name, int length, float* value )
{
    int loc;
    if(GetUniformLocation(program,name,&loc))
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

void ShowShaderLog( Shader shader )
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

void ShowProgramLog( Program program )
{
    GLint length = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);

    char* log = NULL;
    if(length > 1) // See ShowProgramLog
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

GLuint CreateShader( const char* file, int type )
{
    Shader shader = glCreateShader(type);

    int size;
    const char* source = LoadFile(file, &size);
    if(!source)
    {
        Error("Failed to read shader source %s", file);
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
        Log("Compiled program object successfully: %s", file);
    }
    else
    {
        Error("Error compiling program object %s", file);
        return 0;
    }

    return shader;
}

GLuint LoadProgram( const char* vert, const char* frag )
{
    const Shader vertShader = CreateShader(vert, GL_VERTEX_SHADER);
    if(!vertShader)
        return 0;

    const Shader fragShader = CreateShader(frag, GL_FRAGMENT_SHADER);
    if(!fragShader)
        return 0;

    const Program program = glCreateProgram();
    glAttachShader(program, vertShader);
    glAttachShader(program, fragShader);
    BindVertexAttributes(program);

    glLinkProgram(program);
    {
        GLint state;
        glGetProgramiv(program, GL_LINK_STATUS, &state);
        ShowProgramLog(program);

        if(state)
        {
            Log("Linked program program successfully (%s, %s)", vert, frag);
        }
        else
        {
            Error("Error linking program programm (%s, %s)", vert, frag);
            return 0;
        }
    }

    glValidateProgram(program);
    {
        GLint state;
        glGetProgramiv(program, GL_VALIDATE_STATUS, &state);
        ShowProgramLog(program);
        if(state)
            Log("Validated program program successfully (%s, %s)", vert, frag);
        else
            Log("Error validating program program (%s, %s)", vert, frag);
    }

    return program;
}

void BindProgram( Program program )
{
    glUseProgram(program);
}

void FreeProgram( Program program )
{
    glDeleteProgram(program);
}
