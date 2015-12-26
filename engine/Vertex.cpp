#include <assert.h>

#include "OpenGL.h"
#include "Vertex.h"


using namespace glm;


enum
{
    VERTEX_POSITION = 1, // begins with 1 because 0 is an invalid/reserved attribute name
    VERTEX_COLOR,
    VERTEX_TEXCOORD,
    VERTEX_NORMAL,
    VERTEX_TANGENT,
    VERTEX_BITANGENT
};

void EnableVertexArrays()
{
    glEnableVertexAttribArray(VERTEX_POSITION);
    glEnableVertexAttribArray(VERTEX_COLOR);
    glEnableVertexAttribArray(VERTEX_TEXCOORD);
    glEnableVertexAttribArray(VERTEX_NORMAL);
    glEnableVertexAttribArray(VERTEX_TANGENT);
    glEnableVertexAttribArray(VERTEX_BITANGENT);
}

void BindVertexAttributes( unsigned int programHandle )
{
    glBindAttribLocation(programHandle, VERTEX_POSITION, "VertexPosition");
    glBindAttribLocation(programHandle, VERTEX_COLOR,    "VertexColor");
    glBindAttribLocation(programHandle, VERTEX_TEXCOORD, "VertexTexCoord");
    glBindAttribLocation(programHandle, VERTEX_NORMAL,   "VertexNormal");
    glBindAttribLocation(programHandle, VERTEX_TANGENT,  "VertexTangent");
    glBindAttribLocation(programHandle, VERTEX_BITANGENT,"VertexBitangent");
}

void SetVertexAttributePointers( const void* data )
{
    long offset = 0;
#define AttribPointer(Name,Count,TypeName,Type) \
    glVertexAttribPointer( Name, Count, TypeName, GL_TRUE, sizeof(Vertex), reinterpret_cast<const char*>(data)+offset); offset += sizeof( Type ) * Count;
    AttribPointer(VERTEX_POSITION, 3,GL_FLOAT,float);
    AttribPointer(VERTEX_COLOR,    3,GL_FLOAT,float);
    AttribPointer(VERTEX_TEXCOORD, 2,GL_FLOAT,float);
    AttribPointer(VERTEX_NORMAL,   3,GL_FLOAT,float);
    AttribPointer(VERTEX_TANGENT,  3,GL_FLOAT,float);
    AttribPointer(VERTEX_BITANGENT,3,GL_FLOAT,float);
#undef AttribPointer
}

static void CalcVertexTangents( vec3 p1, // position B-A
                                vec3 p2, // position C-A
                                vec2 t1, // texcoord B-A
                                vec2 t2, // texcoord C-A
                                vec3* tangent,
                                vec3* bitangent )
{
    const float coef = 1.0f / (t1[0]*t2[1] - t1[1]*t2[0]);
    *bitangent = (p1*t2[0] - p2*t1[0])*coef;
    *tangent   = (p1*t2[1] - p2*t1[1])*coef;

    // Gram–Schmidt orthogonalization:
    //const vec3 normal = cross(p2, p1);
    //*bitangent = normalize(*bitangent - normal * dot(normal, *bitangent));
    //*tangent   = normalize(  *tangent - normal * dot(normal,   *tangent));
}

void CalcTriangleTangents( Vertex* a, Vertex* b, Vertex* c )
{
    vec3 tangent;
    vec3 bitangent;

    CalcVertexTangents(b->position - a->position,
                       c->position - a->position,
                       b->texCoord - a->texCoord,
                       c->texCoord - a->texCoord,
                       &tangent,
                       &bitangent);

    a->tangent += tangent;
    b->tangent += tangent;
    c->tangent += tangent;

    a->bitangent += bitangent;
    b->bitangent += bitangent;
    c->bitangent += bitangent;
}

void CalcTriangleNormal( Vertex* a, Vertex* b, Vertex* c )
{
    const vec3 p1 = b->position - a->position;
    const vec3 p2 = c->position - a->position;

    //const vec3 normal = cross(p1, p2);
    const vec3 normal = cross(p2, p1);

    a->normal += normal;
    b->normal += normal;
    c->normal += normal;
}
