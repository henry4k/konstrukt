#include "OpenGL.h"
#include "Model.h"

bool Model::Create( Model* model, const Mesh* mesh )
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

bool Model::Load( Model* model, const char* mesh )
{
    Mesh m;
    if(!Mesh::Load(&m, mesh))
        return false;

    bool r = Model::Create(model, &m);

    Mesh::Free(&m);
    return r;
}

void Model::Free( const Model* model )
{
    glDeleteBuffers(1, &model->vertexBuffer);
    glDeleteBuffers(1, &model->indexBuffer);
}

void Model::Draw( const Model* model )
{
    glBindBuffer(GL_ARRAY_BUFFER, model->vertexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->indexBuffer);

    Vertex::SetAttributePointers();
    glDrawElements(model->primitiveType, model->size, GL_UNSIGNED_SHORT, 0);
}
