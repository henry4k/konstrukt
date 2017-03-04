#include <string.h> // memset, strcmp
#include <stdlib.h> // qsort
#include <stdint.h> // uintptr_t

#include "Common.h"
#include "Profiler.h"
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include "Reference.h"
#include "PhysicsManager.h"
#include "Camera.h"
#include "LightWorld.h"
#include "ModelWorld.h"


static const int MAX_MODELS = 64;


struct Model
{
    bool active;
    ReferenceCounter refCounter;
    Mat4 transformation;
    Mesh* mesh;
    char programFamilyList[MAX_PROGRAM_FAMILY_LIST_SIZE];
    ShaderVariableSet* shaderVariableSet;
    Solid* attachmentTarget;
    int attachmentFlags;
    int overlayLevel;
};

struct ModelDrawEntry
{
    const Model* model;
    ShaderProgram* program;
    ShaderVariableSet* generatedVariableSet;
    ShaderVariableBindings bindings;
};

struct ModelWorld
{
    ReferenceCounter refCounter;
    Model models[MAX_MODELS];
    ModelDrawEntry drawEntries[MAX_MODELS];
};


static void FreeModel( Model* model );
static bool ModelIsComplete( const Model* model );
static int CompareModelDrawEntries( const void* a_, const void* b_ );

ModelWorld* CreateModelWorld()
{
    ModelWorld* world = new ModelWorld;
    memset(world, 0, sizeof(ModelWorld));
    InitReferenceCounter(&world->refCounter);
    REPEAT(MAX_MODELS, i)
        world->drawEntries[i].generatedVariableSet = CreateShaderVariableSet();
    return world;
}

static void FreeModelWorld( ModelWorld* world )
{
    REPEAT(MAX_MODELS, i)
    {
        Model* model = &world->models[i];
        if(model->active)
        {
            FatalError("Model #%d (%p) was still active when the world was destroyed.",
                       i, model);
            FreeModel(model);
        }

        FreeShaderVariableSet(world->drawEntries[i].generatedVariableSet);
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

static void ClearModelDrawEntry( ModelDrawEntry* entry )
{
    ClearShaderVariableSet(entry->generatedVariableSet);
    entry->bindings.textureCount = 0;
}

static int GetShaderVariableSets( const ShaderVariableSet*** setsOut,
                                  const ModelDrawEntry* entry,
                                  const Camera* camera )
{
    static const int SHADER_VARIABLE_SET_COUNT = 6;
    static const ShaderVariableSet* sets[SHADER_VARIABLE_SET_COUNT];
    *setsOut = sets;
    const LightWorld* lightWorld = GetCameraLightWorld(camera);
    if(lightWorld) // TODO: Kinda dirty solution :/
    {
        sets[0] = GetCameraShaderVariableSet(camera);
        sets[1] = GetLightWorldShaderVariableSet(lightWorld);
        sets[2] = entry->generatedVariableSet;
        sets[3] = entry->model->shaderVariableSet;
        sets[4] = GetShaderProgramShaderVariableSet(entry->program);
        sets[5] = GetGlobalShaderVariableSet();
        return SHADER_VARIABLE_SET_COUNT;
    }
    else
    {
        sets[0] = GetCameraShaderVariableSet(camera);
        sets[1] = entry->generatedVariableSet;
        sets[2] = entry->model->shaderVariableSet;
        sets[3] = GetShaderProgramShaderVariableSet(entry->program);
        sets[4] = GetGlobalShaderVariableSet();
        return SHADER_VARIABLE_SET_COUNT-1;
    }
}

static Mat4 CalculateModelTransformation( const Model* model )
{
    const Mat4 solidTransformation =
        TryToGetSolidTransformation(model->attachmentTarget,
                                    model->attachmentFlags);
    return MulMat4(solidTransformation, model->transformation);
}

static void SetModelDrawEntry( ModelDrawEntry* entry,
                               const Model* model,
                               const ShaderProgramSet* programSet,
                               const Camera* camera )
{
    entry->model = model;
    entry->program = GetShaderProgramByFamilyList(programSet,
                                                  model->programFamilyList);
    const ShaderVariableSet** variableSets = NULL;

    GenerateCameraModelShaderVariables(camera,
                                       entry->generatedVariableSet,
                                       entry->program,
                                       CalculateModelTransformation(model),
                                       1); // TODO: Pass a proper radius.

    const int variableSetCount =
        GetShaderVariableSets(&variableSets, entry, camera);
    GatherShaderVariableBindings(entry->program,
                                 &entry->bindings,
                                 variableSets,
                                 variableSetCount);
}

static void DrawModel( const ModelDrawEntry* entry,
                       Camera* camera )
{
    const Model* model     = entry->model;
    ShaderProgram* program = entry->program;

    if(!ModelIsComplete(model))
        FatalError("Trying to draw incomplete model %p.", model);

    BindShaderProgram(program);

    // Texture optimization is handled by the texture module already.
    REPEAT(entry->bindings.textureCount, i)
        BindTexture(entry->bindings.textures[i], i);

    const ShaderVariableSet** variableSets = NULL;
    const int variableSetCount =
        GetShaderVariableSets(&variableSets, entry, camera);
    SetShaderProgramUniforms(program,
                             variableSets,
                             variableSetCount,
                             &entry->bindings);

    SetOverlayLevel(model->overlayLevel);

    // Mesh optimization is handled by the mesh module already.
    DrawMesh(model->mesh);
}

void DrawModelWorld( ModelWorld* world,
                     const ShaderProgramSet* programSet,
                     Camera* camera )
{
    ProfileFunction();
    ProfileFunction(GPU_SAMPLE);

    const Model* models = world->models;
    ModelDrawEntry* drawEntries = world->drawEntries;
    int drawEntryCount = 0;

    // Fill draw list:
    REPEAT(MAX_MODELS, i)
    {
        const Model* model = &models[i];
        if(model->active)
        {
            ModelDrawEntry* entry = &drawEntries[drawEntryCount];
            ClearModelDrawEntry(entry);
            SetModelDrawEntry(entry, model, programSet, camera);
            drawEntryCount++;
        }
    }

    // Sort draw list:
    qsort(drawEntries, drawEntryCount, sizeof(ModelDrawEntry), CompareModelDrawEntries);

    // Render draw list:
    REPEAT(drawEntryCount, i)
        DrawModel(&drawEntries[i], camera);

    SetOverlayLevel(0);
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
    // TODO: https://bitsquid.blogspot.de/2017/02/stingray-renderer-walkthrough-4-sorting.html
    // - Sort transparent geometry from back to front,
    //   *BUT* sort solid geometry from front to back -> reduces overdraw


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

    r = Compare((uintptr_t)a->bindings.textures[0],
                (uintptr_t)b->bindings.textures[0]);
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
    if(!model)
        FatalError("Can't create more models.");

    memset(model, 0, sizeof(Model));
    model->active = true;
    InitReferenceCounter(&model->refCounter);
    model->transformation = Mat4Identity;
    model->shaderVariableSet = CreateShaderVariableSet();
    return model;
}

static void FreeModel( Model* model )
{
    model->active = false;
    FreeReferenceCounter(&model->refCounter);
    FreeShaderVariableSet(model->shaderVariableSet);
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

void SetModelProgramFamilyList( Model* model, const char* familyList )
{
    CopyString(familyList,
               model->programFamilyList,
               sizeof(model->programFamilyList));
}

ShaderVariableSet* GetModelShaderVariableSet( const Model* model )
{
    return model->shaderVariableSet;
}

static bool ModelIsComplete( const Model* model )
{
    return model->mesh &&
           model->programFamilyList[0] != '\0';
}
