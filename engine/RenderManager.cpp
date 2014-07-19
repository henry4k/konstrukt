#include <string.h> // memset, strcmp, strncpy

#include "Common.h"
#include "Mesh.h"
#include "Texture.h"
#include "Reference.h"
#include "RenderManager.h"


enum UniformValueSource
{
    DEFAULT_UNIFORM_VALUE = 0,
    LOCAL_UNIFORM_VALUE,
    UNIFORM_VALUE_SOURCE_COUNT
};

struct Model
{
    bool active;
    ReferenceCounter refCounter;
    glm::mat4 transformation;
    Mesh* mesh;
    Texture* texture;
    ShaderProgram* program;
    UniformValue* localUniformValues;
    bool* useLocalUniformValue;
};


static const int MAX_MODELS = 8;
static Model Models[MAX_MODELS];


static void DrawModel( const Model* model, glm::mat4* mvpMatrix );
static void FreeModel( Model* model );
static bool ModelIsComplete( const Model* model );

bool InitRenderManager()
{
    memset(Models, 0, sizeof(Models));
    return true;
}

void DestroyRenderManager()
{
    for(int i = 0; i < MAX_MODELS; i++)
    {
        if(Models[i].active)
        {
            Error("Model #%d (%p) was still active when the manager was destroyed.",
                i, &Models[i]);
            FreeModel(&Models[i]);
        }
    }
}

void DrawModels( glm::mat4 mvpMatrix )
{
    // Naive draw method:
    for(int i = 0; i < MAX_MODELS; i++)
    {
        const Model* model = &Models[i];
        if(model->active)
            DrawModel(model, &mvpMatrix);
    }
}

static void DrawModel( const Model* model, glm::mat4* mvpMatrix )
{
    if(!model->mesh)
        return;

    if(!ModelIsComplete(model))
    {
        Error("Trying to draw incomplete model.");
        return;
    }

    ShaderProgram* program = model->program;
    BindShaderProgram(program);

    BindTexture(model->texture, 0);

    UniformValue mvpUniformValue;
    mvpUniformValue.m4() = *mvpMatrix;
    SetUniformDefault(program, "MVP", &mvpUniformValue);

    const int uniformCount = GetUniformCount(program);
    for(int i = 0; i < uniformCount; i++)
    {
        if(model->useLocalUniformValue[i])
            SetUniform(program, i, &model->localUniformValues[i]);
        else
            ResetUniform(program, i);
    }

    DrawMesh(model->mesh);
}

static Model* FindInactiveModel()
{
    for(int i = 0; i < MAX_MODELS; i++)
        if(!Models[i].active)
            return &Models[i];
    return NULL;
}

Model* CreateModel( ShaderProgram* program )
{
    Model* model = FindInactiveModel();
    if(model)
    {
        memset(model, 0, sizeof(Model));
        model->active = true;
        InitReferenceCounter(&model->refCounter);

        model->program = program;
        ReferenceShaderProgram(program);

        const int uniformCount = GetUniformCount(program);

        model->localUniformValues = new UniformValue[uniformCount];
        memset(model->localUniformValues, 0, sizeof(UniformValue)*uniformCount);

        model->useLocalUniformValue = new bool[uniformCount];
        memset(model->useLocalUniformValue, 0, sizeof(bool)*uniformCount);

        return model;
    }
    else
    {
        Error("Can't create more models.");
        return NULL;
    }
}

static void FreeModel( Model* model )
{
    model->active = false;
    FreeReferenceCounter(&model->refCounter);
    ReleaseShaderProgram(model->program);
    if(model->texture)
        ReleaseTexture(model->texture);
    if(model->mesh)
        ReleaseMesh(model->mesh);
    delete[] model->localUniformValues;
    delete[] model->useLocalUniformValue;
}

void ReferenceModel( Model* model )
{
    Reference(&model->refCounter);
}

void ReleaseModel( Model* model )
{
    Release(&model->refCounter);
    if(!HasReferences(&model->refCounter))
        FreeModel(model);
}

void SetModelTransformation( Model* model, glm::mat4 transformation )
{
    model->transformation = transformation;
}

void SetModelMesh( Model* model, Mesh* mesh )
{
    if(model->mesh)
        ReleaseMesh(model->mesh);
    model->mesh = mesh;
    if(model->mesh)
        ReferenceMesh(model->mesh);
}

void SetModelTexture( Model* model, Texture* texture )
{
    if(model->texture)
        ReleaseTexture(model->texture);
    model->texture = texture;
    if(model->texture)
        ReferenceTexture(model->texture);
}

void SetModelUniform( Model* model, const char* name, UniformValue* value )
{
    const int index = GetUniformIndex(model->program, name);
    if(index != INVALID_UNIFORM_INDEX)
    {
        model->useLocalUniformValue[index] = true;
        model->localUniformValues[index] = *value;
    }
}

void UnsetModelUniform( Model* model, const char* name )
{
    const int index = GetUniformIndex(model->program, name);
    if(index != INVALID_UNIFORM_INDEX)
        model->useLocalUniformValue[index] = false;
}

static bool ModelIsComplete( const Model* model )
{
    return model->mesh &&
           model->texture &&
           model->program;
}
