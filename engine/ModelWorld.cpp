#include <string.h> // memset, strcmp, strncpy
#include <stdlib.h> // qsort

#include "Common.h"
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include "Reference.h"
#include "PhysicsManager.h"
#include "Camera.h"
#include "ModelWorld.h"

using glm::mat4;


static const int MAX_LOCAL_UNIFORMS = 8;
static const int MAX_MODELS = 8;

struct LocalUniform
{
    char name[MAX_UNIFORM_NAME_LENGTH+1];
    UniformType type;
    UniformValue value;
};

struct Model
{
    bool active;
    ReferenceCounter refCounter;
    mat4 transformation;
    Mesh* mesh;
    Texture* textures[MAX_TEXTURE_UNITS];
    char programFamilyList[MAX_PROGRAM_FAMILY_LIST_LENGTH+1];
    LocalUniform uniforms[MAX_LOCAL_UNIFORMS];
    Solid* attachmentTarget;
};

struct ModelDrawEntry
{
    Model* model;
    ShaderProgram* program;
};

struct ModelWorld
{
    ReferenceCounter refCounter;
    Model models[MAX_MODELS];
};


static void FreeModel( Model* model );
static bool ModelIsComplete( const Model* model );
static void SetModelUniforms( const Model* model,
                              ShaderProgram* program,
                              const Camera* camera );
static int CompareModelDrawEntries( const void* a_, const void* b_ );

ModelWorld* CreateModelWorld()
{
    ModelWorld* world = new ModelWorld;
    memset(world, 0, sizeof(ModelWorld));
    InitReferenceCounter(&world->refCounter);
    return world;
}

static void FreeModelWorld( ModelWorld* world )
{
    for(int i = 0; i < MAX_MODELS; i++)
    {
        Model* model = &world->models[i];
        if(model->active)
        {
            Error("Model #%d (%p) was still active when the manager was destroyed.",
                  i, model);
            FreeModel(model);
        }
    }
    delete world;
}

void ReferenceModelWorld( ModelWorld* world )
{
    Reference(&world->refCounter);
}

void ReleaseModelWorld( ModelWorld* world )
{
    Release(&world->refCounter);
    if(!HasReferences(&world->refCounter))
        FreeModelWorld(world);
}

void DrawModelWorld( ModelWorld* world,
                     const ShaderProgramSet* programSet,
                     const Camera* camera )
{
    Model* models = world->models;
    ModelDrawEntry drawList[MAX_MODELS] = {};
    int drawListSize = 0;

    // Fill draw list:
    for(int i = 0; i < MAX_MODELS; i++)
    {
        const Model* model = &models[i];
        if(model->active)
        {
            drawList[i].model = &models[i];
            drawList[i].program =
                GetShaderProgramByFamilyList(programSet,
                                             model->programFamilyList);
            drawListSize++;
        }
    }

    // Sort draw list:
    qsort(drawList, drawListSize, sizeof(ModelDrawEntry*), CompareModelDrawEntries);

    // Render draw list:
    int currentStage = 0;
    ShaderProgram* currentProgram = NULL;
    for(int i = 0; i < drawListSize; i++)
    {
        const Model* model = drawList[i].model;
        ShaderProgram* program = drawList[i].program;

        if(!ModelIsComplete(model))
        {
            Error("Trying to draw incomplete model %p.", model);
            continue;
        }

        if(program != currentProgram)
        {
            currentProgram = program;
            BindShaderProgram(program);
            SetCameraUniforms(camera, program);
        }

        // Texture optimization is handled by the texture module already.
        for(int i = 0; i < MAX_TEXTURE_UNITS; i++)
            if(model->textures[i])
                BindTexture(model->textures[i], i);

        SetModelUniforms(model, program, camera);

        // Mesh optimization is handled by the mesh module already.
        DrawMesh(model->mesh);
    }
}

static mat4 CalculateModelTransformation( const Model* model )
{
    mat4 solidTransformation;
    if(model->attachmentTarget)
        GetSolidTransformation(model->attachmentTarget, &solidTransformation);

    return solidTransformation *
           model->transformation;
}

static void SetModelUniforms( const Model* model,
                              ShaderProgram* program,
                              const Camera* camera )
{
    const mat4 modelTransformation = CalculateModelTransformation(model);
    SetCameraModelUniforms(camera, program, &modelTransformation);

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

static int CompareModelDrawEntries( const void* a_, const void* b_ )
{
    const ModelDrawEntry* a = *(const ModelDrawEntry**)a_;
    const ModelDrawEntry* b = *(const ModelDrawEntry**)b_;

    int r;
    r = Compare((long)a->program,
                (long)b->program);
    if(r != 0)
        return r;
    r = Compare((long)a->model->mesh,
                (long)b->model->mesh);
    if(r != 0)
        return r;
    r = Compare((long)a->model->textures[0],
                (long)b->model->textures[0]);
    if(r != 0)
        return r;
    return 0;
}

static Model* FindInactiveModel( ModelWorld* world )
{
    for(int i = 0; i < MAX_MODELS; i++)
    {
        Model* model = &world->models[i];
        if(!model->active)
            return model;
    }
    return NULL;
}

Model* CreateModel( ModelWorld* world )
{
    Model* model = FindInactiveModel(world);
    if(model)
    {
        memset(model, 0, sizeof(Model));
        model->active = true;
        InitReferenceCounter(&model->refCounter);
        model->transformation = mat4();
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

void SetModelProgramFamilyList( Model* model, const char* familyList )
{
    strncpy(model->programFamilyList, familyList, MAX_PROGRAM_FAMILY_LIST_LENGTH);
    model->programFamilyList[MAX_PROGRAM_FAMILY_LIST_LENGTH] = '\0';
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
    return FindUniform(model, "");
}

void SetModelUniform( Model* model, const char* name, UniformType type, const UniformValue* value )
{
    LocalUniform* uniform = FindUniform(model, name);
    if(!uniform)
        uniform = FindFreeUniform(model);
    if(!uniform)
        FatalError("Too many local uniforms for model %p.", model);

    memset(uniform, 0, sizeof(LocalUniform));

    strncpy(uniform->name, name, MAX_UNIFORM_NAME_LENGTH);
    uniform->type = type;
    memcpy(&uniform->value, value, GetUniformSize(type));
}

void UnsetModelUniform( Model* model, const char* name )
{
    LocalUniform* uniform = FindUniform(model, name);
    if(uniform)
        uniform->name[0] = '\0';
}

static bool ModelIsComplete( const Model* model )
{
    return model->mesh &&
           model->programFamilyList[0] != '\0';
}
