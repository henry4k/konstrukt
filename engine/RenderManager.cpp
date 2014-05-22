#include <string.h> // memset, strcmp, strncpy

#include "Common.h"
#include "Mesh.h"
#include "RenderManager.h"


const int MAX_UNIFORM_NAME_LENGTH = 32;
const int MAX_UNIFORM_COUNT = 4;

enum UniformType
{
    FLOAT_UNIFORM,
    VEC3_UNIFORM,
    VEC4_UNIFORM,
    MAT3_UNIFORM,
    MAT4_UNIFORM
};

struct Uniform
{
    char name[MAX_UNIFORM_NAME_LENGTH];
    UniformType type;
    union value
    {
        float f;
        glm::vec3 v3;
        glm::vec4 v4;
        glm::mat3 m3;
        glm::mat4 m4;
    };
};

struct Model
{
    bool active;
    glm::mat4 transformation;
    Mesh* mesh;
    ShaderProgram program;
    Uniform uniforms[MAX_UNIFORM_COUNT];
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

void SetModelShaderProgram( Model* model, ShaderProgram program )
{
    model->program = program;
}

static Uniform* GetModelUniform( Model* model, const char* name )
{
    for(int i = 0; i < MAX_UNIFORM_COUNT; i++)
        if(strncmp(name, model->uniforms[i].name, MAX_UNIFORM_NAME_LENGTH) == 0)
            return &model->uniforms[i];
    return NULL;
}

static Uniform* CreateOrGetModelUniform( Model* model, const char* name )
{
    Uniform* uniform = GetModelUniform(model, name);
    if(uniform)
    {
        return uniform;
    }
    else
    {
        for(int i = 0; i < MAX_UNIFORM_COUNT; i++)
        {
            if(model->uniforms[i].name[0] == '\0')
            {
                uniform = &model->uniforms[i];
                strncpy(uniform->name, name, MAX_UNIFORM_NAME_LENGTH);
                return uniform;
            }
        }

        Error("Can't create more uniform variables.");
        return NULL;
    }
}

void SetModelFloatUniform( Model* model, const char* name, float value )
{
    Uniform* uniform = CreateOrGetModelUniform(model, name);
    if(uniform)
    {
        uniform->type = FLOAT_UNIFORM;
        uniform->value.f = value;
    }
}

void SetModelVec3Uniform( Model* model, const char* name, glm::vec3 value )
{
    Uniform* uniform = CreateOrGetModelUniform(model, name);
    if(uniform)
    {
        uniform->type = VEC3_UNIFORM;
        uniform->value.v3 = value;
    }
}

void UnsetModelUniform( Model* model, const char* name )
{
    Uniform* uniform = GetModelUniform(model, name);
    if(uniform)
        memset(uniform, 0, sizeof(Uniform));
}