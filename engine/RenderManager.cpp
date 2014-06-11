#include <string.h> // memset, strcmp, strncpy

#include "Common.h"
#include "Mesh.h"
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
    glm::mat4 transformation;
    Mesh* mesh;
    Texture texture;
    ShaderProgram* program;
    UniformValue* localUniformValues;
    bool* useLocalUniformValue;
};


static const int MAX_MODELS = 8;
static Model Models[MAX_MODELS];


static void DrawModel( const Model* model, glm::mat4* mvpMatrix );

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

    ShaderProgram* program = model->program;
    BindShaderProgram(program);

    BindTexture(GL_TEXTURE_2D, model->texture, 0);

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
        model->texture = INVALID_TEXTURE;
        model->program = program;

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

void FreeModel( Model* model )
{
    model->active = false;
    delete[] model->localUniformValues;
    delete[] model->useLocalUniformValue;
}

void SetModelTransformation( Model* model, glm::mat4 transformation )
{
    model->transformation = transformation;
}

void SetModelMesh( Model* model, Mesh* mesh )
{
    model->mesh = mesh;
}

void SetModelTexture( Model* model, Texture texture )
{
    model->texture = texture;
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
