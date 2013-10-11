#include "OpenGL.h"
#include "Vertex.h"

enum
{
    VERTEX_POSITION = 1, // begins with 1 because 0 is an invalid/reserved attribute name
    VERTEX_TEXCOORD,
    VERTEX_NORMAL,
    VERTEX_TANGENT,
    VERTEX_COLOR
};

void Vertex::EnableVertexArrays()
{
    glEnableVertexAttribArray(VERTEX_POSITION);
    glEnableVertexAttribArray(VERTEX_TEXCOORD);
    glEnableVertexAttribArray(VERTEX_NORMAL);
    glEnableVertexAttribArray(VERTEX_TANGENT);
//  glEnableVertexAttribArray(VERTEX_COLOR);
}

void Vertex::BindAttributes( Handle shader )
{
    glBindAttribLocation(shader, VERTEX_POSITION, "VertexPosition");
    glBindAttribLocation(shader, VERTEX_TEXCOORD, "VertexTexCoord");
    glBindAttribLocation(shader, VERTEX_NORMAL,   "VertexNormal");
    glBindAttribLocation(shader, VERTEX_TANGENT,  "VertexTangent");
//  glBindAttribLocation(shader, VERTEX_COLOR,    "VertexColor");
}

void Vertex::SetAttributePointers()
{
    long offset = 0;
#define AttribPointer(Name,Count,TypeName,Type) glVertexAttribPointer( Name , Count , TypeName, GL_TRUE, sizeof(Vertex), (void*)offset); offset += sizeof( Type ) * Count;
    AttribPointer(VERTEX_POSITION,3,GL_FLOAT,float);
    AttribPointer(VERTEX_TEXCOORD,2,GL_FLOAT,float);
    AttribPointer(VERTEX_NORMAL,3,GL_FLOAT,float);
    AttribPointer(VERTEX_TANGENT,4,GL_FLOAT,float);
//  AttribPointer(VERTEX_COLOR,4,GL_UNSIGNED_BYTE,unsigned char);
#undef AttribPointer
}
