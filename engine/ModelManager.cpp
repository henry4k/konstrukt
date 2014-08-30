#include <string.h> // memset, strcmp, strncpy

#include "Math.h"
#include <glm/gtc/matrix_transform.hpp>
#include "Common.h"
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include "Reference.h"
#include "PhysicsManager.h"
#include "ModelManager.h"


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
    Solid* attachmentTarget;
};


static const int MAX_MODELS = 8;
static Model Models[STAGE_COUNT][MAX_MODELS];

static void DrawModel( const Model* model, const glm::mat4* transformations );
static void FreeModel( Model* model );
static bool ModelIsComplete( const Model* model );
static const char* GetStageName( ModelStage stage );

bool InitModelManager()
{
    memset(Models, 0, sizeof(Models));
    return true;
}

void DestroyModelManager()
{
    for(int stage = 0; stage < STAGE_COUNT; stage++)
    for(int i = 0; i < MAX_MODELS; i++)
    {
        Model* model = &Models[stage][i];
        if(model->active)
        {
            Error("%s model #%d (%p) was still active when the manager was destroyed.",
                GetStageName((ModelStage)stage), i, model);
            FreeModel(model);
        }
    }
}

void DrawModels( const glm::mat4* projectionTransformation,
                 const glm::mat4* viewTransformation,
                 const glm::mat4* modelTransformation )
{
    // Draw background:
    const glm::mat4 backgroundMVP = *projectionTransformation *
                                    *viewTransformation;
    for(int i = 0; i < MAX_MODELS; i++)
    {
        const Model* model = &Models[BACKGROUND_STAGE][i];
        if(model->active)
            DrawModel(model, &backgroundMVP);
    }

    // Draw world:
    glClear(GL_DEPTH_BUFFER_BIT);
    const glm::mat4 worldMVP = *projectionTransformation *
                               *viewTransformation *
                               *modelTransformation;
    for(int i = 0; i < MAX_MODELS; i++)
    {
        const Model* model = &Models[WORLD_STAGE][i];
        if(model->active)
            DrawModel(model, &worldMVP);
    }

    // Draw HUD:
    glClear(GL_DEPTH_BUFFER_BIT);
    const glm::mat4 hudMVP = glm::scale(*projectionTransformation,
                                        glm::vec3(1, 1, -1));
    for(int i = 0; i < MAX_MODELS; i++)
    {
        const Model* model = &Models[HUD_STAGE][i];
        if(model->active)
            DrawModel(model, &hudMVP);
    }
}

static void DrawModel( const Model* model, const glm::mat4* mvpMatrix )
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

    glm::mat4 solidTransformation;
    if(model->attachmentTarget)
        GetSolidTransformation(model->attachmentTarget, &solidTransformation);

    mvpUniformValue.m4() = *mvpMatrix *
                           solidTransformation *
                           model->transformation;
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

static Model* FindInactiveModel( ModelStage stage )
{
    for(int i = 0; i < MAX_MODELS; i++)
    {
        Model* model = &Models[stage][i];
        if(!model->active)
            return model;
    }
    return NULL;
}

Model* CreateModel( ModelStage stage, ShaderProgram* program )
{
    Model* model = FindInactiveModel(stage);
    if(model)
    {
        memset(model, 0, sizeof(Model));
        model->active = true;
        InitReferenceCounter(&model->refCounter);

        model->transformation = glm::mat4();

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
        Error("Can't create more %s models.", GetStageName(stage));
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
    if(model->attachmentTarget)
        ReleaseSolid(model->attachmentTarget);
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

void SetModelAttachmentTarget( Model* model, Solid* target )
{
    if(model->attachmentTarget)
        ReleaseSolid(model->attachmentTarget);
    model->attachmentTarget = target;
    if(model->attachmentTarget)
        ReferenceSolid(model->attachmentTarget);
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

static const char* GetStageName( ModelStage stage )
{
    switch(stage)
    {
        case WORLD_STAGE: return "world";
        case BACKGROUND_STAGE: return "background";
        case HUD_STAGE: return "hud";
        default: FatalError("Unknown stage."); return NULL;
    }
}
