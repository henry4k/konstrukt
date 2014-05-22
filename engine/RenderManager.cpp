#include <string.h> // memset

#include "Common.h"
#include "Mesh.h"
#include "Shader.h"
#include "RenderManager.h"


struct Model
{
    bool active;
    glm::mat4 transformation;
    Mesh* mesh;
};


static const int MAX_MODELS = 8;
static Model Models[MAX_MODELS];

static ShaderProgram DefaultProgram;

bool InitRenderManager()
{
    memset(Models, 0, sizeof(Models));

    const ShaderObject shaderObjects[] = {
        LoadShaderObject("core/Shaders/Test.vert"),
        LoadShaderObject("core/Shaders/Test.frag")
    };
    DefaultProgram = LinkShaderProgram(shaderObjects, sizeof(shaderObjects)/sizeof(ShaderObject));
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

    BindShaderProgram(DefaultProgram);
    SetUniformMatrix4(DefaultProgram, "MVP", &mvpMatrix);

    for(int i = 0; i < MAX_MODELS; i++)
    {
        const Model* model = &Models[i];
        if(model->active)
        {
            DrawMesh(model->mesh);
        }
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
}

void SetModelTransformation( Model* model, glm::mat4 transformation )
{
    model->transformation = transformation;
}

void SetModelMesh( Model* model, Mesh* mesh )
{
    model->mesh = mesh;
}