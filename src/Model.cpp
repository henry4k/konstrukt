#include "OpenGL.h"
#include "Model.h"

bool CreateModel( Model* model, const Mesh* mesh )
{
    glGenBuffers(1, &model->vertexBuffer);
    glGenBuffers(1, &model->indexBuffer);

    glBindBuffer(GL_ARRAY_BUFFER, model->vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, mesh->vertexCount*sizeof(Vertex), mesh->vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indexCount*sizeof(unsigned short), mesh->indices, GL_STATIC_DRAW);

    model->size = mesh->indexCount;
    model->primitiveType = GL_TRIANGLES;

    return true;
}

bool LoadModel( Model* model, const char* mesh )
{
    Mesh m;
    if(!LoadMesh(&m, mesh))
        return false;

    bool r = CreateModel(model, &m);

    FreeMesh(&m);
    return r;
}

void FreeModel( const Model* model )
{
    glDeleteBuffers(1, &model->vertexBuffer);
    glDeleteBuffers(1, &model->indexBuffer);
}

void DrawModel( const Model* model )
{
    glBindBuffer(GL_ARRAY_BUFFER, model->vertexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->indexBuffer);

    SetVertexAttributePointers();
    glDrawElements(model->primitiveType, model->size, GL_UNSIGNED_SHORT, 0);
}
