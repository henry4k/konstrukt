#include <string.h> // memset, strcmp
#include <stdlib.h> // qsort
#include <stdint.h> // uintptr_t

#include "Common.h"
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include "Reference.h"
#include "PhysicsManager.h"
#include "Camera.h"
#include "ModelWorld.h"


static const int MAX_LOCAL_UNIFORMS = 32;
static const int MAX_MODELS = 64;


struct LocalUniform
{
    char name[MAX_UNIFORM_NAME_SIZE];
    UniformType type;
    UniformValue value;
};

struct Model
{
    bool active;
    ReferenceCounter refCounter;
    Mat4 transformation;
    Mesh* mesh;
    Texture* textures[MAX_TEXTURE_UNITS];
    char programFamilyList[MAX_PROGRAM_FAMILY_LIST_SIZE];
    LocalUniform uniforms[MAX_LOCAL_UNIFORMS];
    //UniformSet* uniformSet;
    Solid* attachmentTarget;
    int attachmentFlags;
    int overlayLevel;
};

struct ModelWorld
{
    ReferenceCounter refCounter;
    Model models[MAX_MODELS];
};

struct ModelDrawEntry
{
    const Model* model;
    ShaderProgram* program;
    //Texture* textures[MAX_TEXTURE_UNITS];
};


static void FreeModel( Model* model );
static bool ModelIsComplete( const Model* model );
static void SetModelUniforms( const Model* model,
                              ShaderProgram* program,
                              Camera* camera );
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
    REPEAT(MAX_MODELS, i)
    {
        Model* model = &world->models[i];
        if(model->active)
        {
            Error("Model #%d (%p) was still active when the world was destroyed.",
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

static int CurrentOverlayLevel = 0;

static void SetOverlayLevel( int level )
{
    if(level == CurrentOverlayLevel)
        return;

    if(CurrentOverlayLevel == 0 &&
                     level != 0)
    {
        glDepthMask(GL_FALSE);

        glEnable(GL_POLYGON_OFFSET_FILL);
        //glEnable(GL_POLYGON_OFFSET_LINE);
        //glEnable(GL_POLYGON_OFFSET_POINT);

    }
    else if(CurrentOverlayLevel != 0 &&
                          level == 0)
    {
        glDepthMask(GL_TRUE);

        glDisable(GL_POLYGON_OFFSET_FILL);
        //glDisable(GL_POLYGON_OFFSET_LINE);
        //glDisable(GL_POLYGON_OFFSET_POINT);
    }

    if(level != 0)
        glPolygonOffset(0.0, -level);

    CurrentOverlayLevel = level;
}

void DrawModelWorld( const ModelWorld* world,
                     const ShaderProgramSet* programSet,
                     Camera* camera )
{
    const Model* models = world->models;
    ModelDrawEntry drawList[MAX_MODELS];
    memset(drawList, 0, sizeof(drawList));
    int drawListSize = 0;

    // Fill draw list:
    REPEAT(MAX_MODELS, i)
    {
        const Model* model = &models[i];
        if(model->active)
        {
            ModelDrawEntry* entry = &drawList[drawListSize];
            entry->model = model;
            entry->program = GetShaderProgramByFamilyList(programSet,
                                                          model->programFamilyList);
            //entry->textures =
            // TODO
            drawListSize++;
        }
    }

    // Sort draw list:
    qsort(drawList, drawListSize, sizeof(ModelDrawEntry), CompareModelDrawEntries);

    // Render draw list:
    ShaderProgram* currentProgram = NULL;
    REPEAT(drawListSize, i)
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
        REPEAT(MAX_TEXTURE_UNITS, i)
            if(model->textures[i])
                BindTexture(model->textures[i], i);

        SetModelUniforms(model, program, camera);

        SetOverlayLevel(model->overlayLevel);

        // Mesh optimization is handled by the mesh module already.
        DrawMesh(model->mesh);
    }

    SetOverlayLevel(0);
}

static Mat4 CalculateModelTransformation( const Model* model )
{
    const Mat4 solidTransformation =
        TryToGetSolidTransformation(model->attachmentTarget,
                                    model->attachmentFlags);
    return MulMat4(solidTransformation, model->transformation);
}

static void SetModelUniforms( const Model* model,
                              ShaderProgram* program,
                              Camera* camera )
{
    const Mat4 modelTransformation = CalculateModelTransformation(model);
    SetCameraModelUniforms(camera, program, &modelTransformation);

    for(int i = 0; i < MAX_LOCAL_UNIFORMS; i++)
    {
        const LocalUniform* uniform = &model->uniforms[i];
        if(uniform->name[0] != '\0')
            SetUniform(program, uniform->name, uniform->type, &uniform->value);
    }
}

static int Compare( uintptr_t a, uintptr_t b )
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
    const ModelDrawEntry* a = (const ModelDrawEntry*)a_;
    const ModelDrawEntry* b = (const ModelDrawEntry*)b_;

    int r;

    r = Compare((uintptr_t)a->model->overlayLevel,
                (uintptr_t)b->model->overlayLevel);
    if(r != 0)
        return r;

    r = Compare((uintptr_t)a->program,
                (uintptr_t)b->program);
    if(r != 0)
        return r;

    r = Compare((uintptr_t)a->model->mesh,
                (uintptr_t)b->model->mesh);
    if(r != 0)
        return r;

    //r = Compare((uintptr_t)a->textures[0],
    //            (uintptr_t)b->textures[0]);
    r = Compare((uintptr_t)a->model->textures[0],
                (uintptr_t)b->model->textures[0]);
    if(r != 0)
        return r;

    return 0;
}

static Model* FindInactiveModel( ModelWorld* world )
{
    REPEAT(MAX_MODELS, i)
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
        model->transformation = Mat4Identity;
        //model->uniformSet = CreateUniformSet();
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
    REPEAT(MAX_TEXTURE_UNITS, i)
        if(model->textures[i])
            ReleaseTexture(model->textures[i]);
    //FreeUniformSet(model->uniformSet);
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

void SetModelAttachmentTarget( Model* model, Solid* target, int flags )
{
    if(model->attachmentTarget)
        ReleaseSolid(model->attachmentTarget);
    model->attachmentTarget = target;
    model->attachmentFlags = flags;
    if(model->attachmentTarget)
        ReferenceSolid(model->attachmentTarget);
}

void SetModelTransformation( Model* model, Mat4 transformation )
{
    model->transformation = transformation;
}

void SetModelOverlayLevel( Model* model, int level )
{
    model->overlayLevel = level;
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
    CopyString(familyList,
               model->programFamilyList,
               sizeof(model->programFamilyList));
}

//UniformSet* GetModelUniformSet( Model* model )
//{
//    return model->uniformSet;
//}

static LocalUniform* FindUniform( Model* model, const char* name )
{
    LocalUniform* uniforms = model->uniforms;
    for(int i = 0; i < MAX_LOCAL_UNIFORMS; i++)
        if(strncmp(name, uniforms[i].name, MAX_UNIFORM_NAME_SIZE-1) == 0)
            return &uniforms[i];
    return NULL;
}

static LocalUniform* FindFreeUniform( Model* model )
{
    return FindUniform(model, "");
}

void SetModelUniform( Model* model,
                      const char* name,
                      UniformType type,
                      const UniformValue* value )
{
    LocalUniform* uniform = FindUniform(model, name);
    if(!uniform)
        uniform = FindFreeUniform(model);
    if(!uniform)
        FatalError("Too many uniforms for model %p.", model);

    memset(uniform, 0, sizeof(LocalUniform));

    CopyString(name, uniform->name, sizeof(uniform->name));
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
