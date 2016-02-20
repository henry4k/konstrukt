#include <assert.h>

#include "OpenGL.h"
#include "Vertex.h"


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

static void CalcVertexTangents( Vec3 p1, // position B-A
                                Vec3 p2, // position C-A
                                Vec2 t1, // texcoord B-A
                                Vec2 t2, // texcoord C-A
                                Vec3* tangent,
                                Vec3* bitangent )
{
    const float coef = 1.f / (t1._[0]*t2._[1] - t1._[1]*t2._[0]);
    REPEAT(3,i) {   tangent->_[i] = (p1._[i]*t2._[1] - p2._[i]*t1._[1])*coef; }
    REPEAT(3,i) { bitangent->_[i] = (p2._[i]*t1._[0] - p1._[i]*t2._[0])*coef; }
}

static void PostprocessVertexTangents( Vertex* v )
{
    const Vec3* normal    = &v->normal;
          Vec3* tangent   = &v->tangent;
          Vec3* bitangent = &v->bitangent;

    // Gram–Schmidt orthogonalization:
    const float nDotT = DotProductOfVec3(*normal, *tangent);
    const float nDotB = DotProductOfVec3(*normal, *bitangent);
    REPEAT(3,i) {   tangent->_[i] =   tangent->_[i] - normal->_[i] * nDotT; }
    REPEAT(3,i) { bitangent->_[i] = bitangent->_[i] - normal->_[i] * nDotB; }

    // Calculate handedness:
    //if(DotProductOfVec3(CrossProductOfVec3(*normal, *tangent), *bitangent) < 0)
    //    REPEAT(3,i) { tangent->_[i] = -tangent->_[i]; }
    // See: http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-13-normal-mapping/
    // But it currently doesn't seem to work.  Probably because the tutorial
    // uses a right handed coordinate system and we use a left handed one.
    // TODO: Investigate as soon as problems arise.
}

#define ADD_TO_VEC3(T,S) REPEAT(3,i) { (T)._[i] += (S)._[i]; }

void CalcTriangleTangents( Vertex* a, Vertex* b, Vertex* c )
{
    Vec3 tangent;
    Vec3 bitangent;

    Vec3 baPosDelta;
    Vec3 caPosDelta;
    Vec2 baTexDelta;
    Vec2 caTexDelta;
    REPEAT(3,i) { baPosDelta._[i] = b->position._[i] - a->position._[i]; }
    REPEAT(3,i) { caPosDelta._[i] = c->position._[i] - a->position._[i]; }
    REPEAT(2,i) { baTexDelta._[i] = b->texCoord._[i] - a->texCoord._[i]; }
    REPEAT(2,i) { caTexDelta._[i] = c->texCoord._[i] - a->texCoord._[i]; }

    CalcVertexTangents(baPosDelta,
                       caPosDelta,
                       baTexDelta,
                       caTexDelta,
                       &tangent,
                       &bitangent);

    ADD_TO_VEC3(a->tangent, tangent)
    ADD_TO_VEC3(b->tangent, tangent)
    ADD_TO_VEC3(c->tangent, tangent)

    ADD_TO_VEC3(a->bitangent, bitangent)
    ADD_TO_VEC3(b->bitangent, bitangent)
    ADD_TO_VEC3(c->bitangent, bitangent)

    PostprocessVertexTangents(a);
    PostprocessVertexTangents(b);
    PostprocessVertexTangents(c);
}

void CalcTriangleNormal( Vertex* a, Vertex* b, Vertex* c )
{
    Vec3 p1, p2;
    REPEAT(3,i) { p1._[i] = b->position._[i] - a->position._[i]; }
    REPEAT(3,i) { p2._[i] = c->position._[i] - a->position._[i]; }

    const Vec3 normal = CrossProductOfVec3(p2, p1);

    ADD_TO_VEC3(a->normal, normal)
    ADD_TO_VEC3(b->normal, normal)
    ADD_TO_VEC3(c->normal, normal)
}
