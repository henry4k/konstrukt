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
    ShaderProgram* program;
    UniformValue* localUniformValues;
    UniformValueSource* uniformValueSources;
};


static const int MAX_MODELS = 8;
static Model Models[MAX_MODELS];

static ShaderProgram DefaultProgram;

bool InitRenderManager()
{
    memset(Models, 0, sizeof(Models));

    const Shader shaders[] = {
        LoadShader("core/Shaders/Test.vert"),
        LoadShader("core/Shaders/Test.frag")
    };
    DefaultProgram = LinkShaderProgram(shaders, sizeof(shaders)/sizeof(Shader));
    BindVertexAttributes(DefaultProgram);

    return true;
}

void DestroyRenderManager()
{
    FreeShaderProgram(DefaultProgram);

    for(int i = 0; i < MAX_MODELS; i++)
        if(Models[i].active)
            Error("Model #%d (%p) was still active when the manager was destroyed.",
                i, &Models[i]);
}

void DrawModels( glm::mat4 mvpMatrix )
{
    // Naive draw method:
    for(int i = 0; i < MAX_MODELS; i++)
    {
        const Model* model = &Models[i];
        if(model->active)
            DrawModel(model);
    }
}

static Model* FindInactiveModel()
{
    for(int i = 0; i < MAX_MODELS; i++)
        if(!Models[i].active)
            return &Models[i];
    return NULL;
}

Model* CreateModel()
{
    Model* model = FindInactiveModel();
    if(model)
    {
        memset(model, 0, sizeof(Model));
        model->active = true;
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

    if(model->localUniformValues)
    {
        delete[] model->localUniformValues;
        model->localUniformValues = NULL;
    }

    if(model->uniformValueSources)
    {
        delete[] model->uniformValueSources;
        model->uniformValueSources = NULL;
    }
}

void SetModelTransformation( Model* model, glm::mat4 transformation )
{
    model->transformation = transformation;
}

void SetModelMesh( Model* model, Mesh* mesh )
{
    model->mesh = mesh;
}

void SetModelShaderProgram( Model* model, ShaderProgram* program )
{
    model->program = program;

    if(model->localUniformValues)
    {
        delete[] model->localUniformValues;
        model->localUniformValues = NULL;
    }

    if(model->uniformValueSources)
    {
        delete[] model->uniformValueSources;
        model->uniformValueSources = NULL;
    }

    const int uniformCount = program->uniformCount;

    model->localUniformValues = new UniformValue[uniformCount];
    memset(model->localUniformValues, 0, sizeof(UniformValue)*uniformCount);

    model->uniformValueSources = new UniformValueSource[uniformCount];
    memset(model->uniformValueSources, 0, sizeof(UniformValueSource)*uniformCount);
}

void SetModelUniform( Model* model, const char* name, UniformValue* value )
{
    const int index = GetUniformIndex(model->program, name);
    if(index != INVALID_UNIFORM_INDEX)
    {
        model->uniformValueSources[index] = LOCAL_UNIFORM_VALUE;
        model->localUniformValues[index] = *value;
    }
}

void UnsetModelUniform( Model* model, const char* name )
{
    const int index = GetUniformIndex(model->program, name);
    if(index != INVALID_UNIFORM_INDEX)
        model->uniformValueSources[index] = DEFAULT_UNIFORM_VALUE;
}

void DrawModel( const Model* model )
{
    const ShaderProgram* program = model->program;
    const int uniformCount = program->uniformCount;

    BindShaderProgram(program);

    const UniformValue* uniformValueSources[UNIFORM_VALUE_SOURCE_COUNT] =
    {
        program->defaultUniformValues, // DEFAULT_UNIFORM_VALUE
        model->localUniformValues      // LOCAL_UNIFORM_VALUE
    };

    for(int i = 0; i < uniformCount; i++)
    {
        const int source = model->uniformValueSources[i];

        SetUniformValueInShaderProgram(
            program,
            i,
            uniformValueSources[source][i]
        );
    }

    DrawMesh(model->mesh);
}