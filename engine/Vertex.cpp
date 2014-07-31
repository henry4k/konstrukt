#include "OpenGL.h"
#include "Vertex.h"

enum
{
    VERTEX_POSITION = 1, // begins with 1 because 0 is an invalid/reserved attribute name
    VERTEX_COLOR,
    VERTEX_TEXCOORD,
    VERTEX_NORMAL,
    VERTEX_TANGENT
};

void EnableVertexArrays()
{
    glEnableVertexAttribArray(VERTEX_POSITION);
    glEnableVertexAttribArray(VERTEX_COLOR);
    glEnableVertexAttribArray(VERTEX_TEXCOORD);
    glEnableVertexAttribArray(VERTEX_NORMAL);
    glEnableVertexAttribArray(VERTEX_TANGENT);
}

void BindVertexAttributes( unsigned int programHandle )
{
    glBindAttribLocation(programHandle, VERTEX_POSITION, "VertexPosition");
    glBindAttribLocation(programHandle, VERTEX_COLOR,    "VertexColor");
    glBindAttribLocation(programHandle, VERTEX_TEXCOORD, "VertexTexCoord");
    glBindAttribLocation(programHandle, VERTEX_NORMAL,   "VertexNormal");
    glBindAttribLocation(programHandle, VERTEX_TANGENT,  "VertexTangent");
}

void SetVertexAttributePointers( const void* data )
{
    long offset = 0;
#define AttribPointer(Name,Count,TypeName,Type) \
    glVertexAttribPointer( Name, Count, TypeName, GL_TRUE, sizeof(Vertex), reinterpret_cast<const char*>(data)+offset); offset += sizeof( Type ) * Count;
    AttribPointer(VERTEX_POSITION,3,GL_FLOAT,float);
    AttribPointer(VERTEX_COLOR,3,GL_FLOAT,float);
    AttribPointer(VERTEX_TEXCOORD,2,GL_FLOAT,float);
    AttribPointer(VERTEX_NORMAL,3,GL_FLOAT,float);
    AttribPointer(VERTEX_TANGENT,4,GL_FLOAT,float);
#undef AttribPointer
}
