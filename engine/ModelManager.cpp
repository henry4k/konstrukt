#include <string.h> // memset, strcmp, strncpy
#include <stdlib.h> // qsort

#include "Math.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include "Common.h"
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include "Reference.h"
#include "PhysicsManager.h"
#include "ModelManager.h"

using glm::mat4;


static const int MAX_LOCAL_UNIFORMS = 8;

struct LocalUniform
{
    bool active;
    char name[MAX_UNIFORM_NAME_LENGTH+1];
    UniformType type;
    UniformValue value;
};

struct Model
{
    bool active;
    ReferenceCounter refCounter;
    int stage;
    mat4 transformation;
    Mesh* mesh;
    Texture* textures[MAX_TEXTURE_UNITS];
    ShaderProgram* program;
    LocalUniform uniforms[MAX_LOCAL_UNIFORMS];
    Solid* attachmentTarget;
};


static const int MAX_MODELS = 8;
static Model Models[MAX_MODELS];

static void FreeModel( Model* model );
static bool ModelIsComplete( const Model* model );

bool InitModelManager()
{
    memset(Models, 0, sizeof(Models));
    return true;
}

void DestroyModelManager()
{
    for(int i = 0; i < MAX_MODELS; i++)
    {
        Model* model = &Models[i];
        if(model->active)
        {
            Error("Model #%d (%p) was still active when the manager was destroyed.",
                  i, model);
            FreeModel(model);
        }
    }
}

static void SetLocalUniforms( ShaderProgram* program,
                               const mat4* projectionTransformation,
                               const mat4* viewTransformation )
{
    const mat4 viewInverseTranspose = glm::inverseTranspose(*viewTransformation);

    SetGlobalUniform("Projection",
                     MAT4_UNIFORM,
                     (const UniformValue*)projectionTransformation);
    SetGlobalUniform("View",
                     MAT4_UNIFORM,
                     (const UniformValue*)viewTransformation);
    SetGlobalUniform("ViewInverseTranspose",
                     MAT4_UNIFORM,
                     (const UniformValue*)&viewInverseTranspose);
}

static mat4 CalculateFinalModelTransformation( const Model* model,
                                               const mat4* modelTransformation )
{
    mat4 solidTransformation;
    if(model->attachmentTarget)
        GetSolidTransformation(model->attachmentTarget, &solidTransformation);

    return *modelTransformation * solidTransformation * model->transformation;
}

static void SetModelUniforms( const Model* model,
                              const mat4* projectionTransformation,
                              const mat4* viewTransformation,
                              const mat4* modelTransformation )
{
    ShaderProgram* program = model->program;

    const mat4 finalModelTransformation =
        CalculateFinalModelTransformation(model, modelTransformation);

    const mat4 modelView = *viewTransformation * finalModelTransformation;
    const mat4 mvp = *projectionTransformation * modelView;
    const mat4 modelViewInverseTranspose = glm::inverseTranspose(modelView);

    SetGlobalUniform("Model", // TODO: Is this actually used in shaders?
                     MAT4_UNIFORM,
                     (const UniformValue*)&finalModelTransformation);
    SetGlobalUniform("ModelView",
                     MAT4_UNIFORM,
                     (const UniformValue*)&modelView);
    SetGlobalUniform("MVP",
                     MAT4_UNIFORM,
                     (const UniformValue*)&mvp);
    SetGlobalUniform("ModelViewInverseTranspose",
                     MAT4_UNIFORM,
                     (const UniformValue*)&modelViewInverseTranspose);
}

static void ApplyModelUniforms( const Model* model )
{
    ShaderProgram* program = model->program;
    for(int i = 0; i < MAX_LOCAL_UNIFORMS; i++)
    {
        const LocalUniform* uniform = &model->uniforms[i];
        if(uniform->active)
            SetUniform(program, uniform->name, uniform->type, &uniform->value);
    }
}

static int Compare( long a, long b )
{
    if(a == b)
        return 0;
    else if(a < b)
        return -1;
    else
        return 1;
}

static int CompareModels( const void* a_, const void* b_ )
{
    const Model* a = *(const Model**)a_;
    const Model* b = *(const Model**)b_;

    int r;
    r = Compare(a->stage, b->stage);
    if(r != 0)
        return r;
    r = Compare((long)a->program, (long)b->program);
    if(r != 0)
        return r;
    r = Compare((long)a->mesh, (long)b->mesh);
    if(r != 0)
        return r;
    r = Compare((long)a->textures[0], (long)b->textures[0]);
    if(r != 0)
        return r;
    return 0;
}

void DrawModels( const mat4* projectionTransformation,
                 const mat4* viewTransformation,
                 const mat4* modelTransformation )
{
    const Model* drawList[MAX_MODELS];
    int drawListSize = 0;

    // Fill draw list:
    for(int i = 0; i < MAX_MODELS; i++)
    {
        const Model* model = &Models[i];
        if(model->active)
        {
            drawList[i] = &Models[i];
            drawListSize++;
        }
    }

    // Sort draw list:
    qsort(drawList, drawListSize, sizeof(Model*), CompareModels);

    // Render draw list:
    int currentStage = 0;
    ShaderProgram* currentProgram = NULL;
    for(int i = 0; i < drawListSize; i++)
    {
        const Model* model = drawList[i];

        if(!ModelIsComplete(model))
        {
            Error("Trying to draw incomplete model %p.", model);
            continue;
        }

        if(model->stage != currentStage)
        {
            glClear(GL_DEPTH_BUFFER_BIT);
            currentStage = model->stage;
        }

        //if(model->program != currentProgram)
        {
            SetLocalUniforms(model->program,
                              projectionTransformation,
                              viewTransformation);
            BindShaderProgram(model->program);
            currentProgram = model->program;
        }

        // Texture optimization is handled by the texture module already.
        for(int i = 0; i < MAX_TEXTURE_UNITS; i++)
            if(model->textures[i])
                BindTexture(model->textures[i], i);

        SetModelUniforms(model,
                         projectionTransformation,
                         viewTransformation,
                         modelTransformation);
        ApplyModelUniforms(model);

        // Mesh optimization is handled by the mesh module already.
        DrawMesh(model->mesh);
    }
}

static Model* FindInactiveModel()
{
    for(int i = 0; i < MAX_MODELS; i++)
    {
        Model* model = &Models[i];
        if(!model->active)
            return model;
    }
    return NULL;
}

Model* CreateModel( ModelStage stage, ShaderProgram* program )
{
    Model* model = FindInactiveModel();
    if(model)
    {
        memset(model, 0, sizeof(Model));
        model->active = true;
        InitReferenceCounter(&model->refCounter);

        model->stage = stage;

        model->transformation = mat4();

        model->program = program;
        ReferenceShaderProgram(program);

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
    for(int i = 0; i < MAX_TEXTURE_UNITS; i++)
        if(model->textures[i])
            ReleaseTexture(model->textures[i]);
    if(model->mesh)
        ReleaseMesh(model->mesh);
    if(model->attachmentTarget)
        ReleaseSolid(model->attachmentTarget);
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

void SetModelTransformation( Model* model, mat4 transformation )
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

void SetModelTexture( Model* model, int unit, Texture* texture )
{
    if(model->textures[unit])
        ReleaseTexture(model->textures[unit]);
    model->textures[unit] = texture;
    if(model->textures[unit])
        ReferenceTexture(model->textures[unit]);
}

static LocalUniform* FindUniform( Model* model, const char* name )
{
    LocalUniform* uniforms = model->uniforms;
    for(int i = 0; i < MAX_LOCAL_UNIFORMS; i++)
        if(strncmp(name, uniforms[i].name, MAX_UNIFORM_NAME_LENGTH) == 0)
            return &uniforms[i];
    return NULL;
}

static LocalUniform* FindFreeUniform( Model* model )
{
    LocalUniform* uniforms = model->uniforms;
    for(int i = 0; i < MAX_LOCAL_UNIFORMS; i++)
        if(!uniforms[i].active)
            return &uniforms[i];
    return NULL;
}

void SetModelUniform( Model* model, const char* name, UniformType type, const UniformValue* value )
{
    LocalUniform* uniform = FindUniform(model, name);
    if(!uniform)
        uniform = FindFreeUniform(model);
    if(!uniform)
        FatalError("Too many local uniforms for model %p.", model);

    memset(uniform, 0, sizeof(LocalUniform));

    uniform->active = true;
    strncpy(uniform->name, name, MAX_UNIFORM_NAME_LENGTH);
    uniform->type = type;
    memcpy(&uniform->value, value, GetUniformSize(type));
}

void UnsetModelUniform( Model* model, const char* name )
{
    LocalUniform* uniform = FindUniform(model, name);
    if(uniform)
        uniform->active = false;
}

static bool ModelIsComplete( const Model* model )
{
    return model->mesh &&
           model->program;
}
