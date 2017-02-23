#include <string.h> // memset, strcmp
#include <stdlib.h> // qsort

#include "Common.h"
#include "Profiler.h"
#include "Reference.h"
#include "PhysicsManager.h"
#include "Shader.h"
#include "LightWorld.h"


static const int MAX_LIGHTS = 32;


struct Light
{
    bool active;
    ReferenceCounter refCounter;
    ShaderVariableSet* shaderVariableSet;
    Solid* attachmentTarget;
    int attachmentFlags;
    Mat4 transformation;

    LightType type;
    float value;
    Vec3 position; // calculated from transformation and attachment target
    float range;
};

struct LightWorld
{
    ReferenceCounter refCounter;
    char lightCountUniformName[MAX_UNIFORM_NAME_SIZE];
    char lightPositionName[MAX_UNIFORM_NAME_SIZE];
    int maxActiveLightCount;
    Light lights[MAX_LIGHTS];
    ShaderVariableSet* shaderVariableSet;
    ShaderVariableSet* unusedLightShaderVariableSet;
};

struct ActiveLight
{
    const Light* light;
    float illuminance;
};


static void FreeLight( Light* light );


LightWorld* CreateLightWorld( const char* lightCountUniformName,
                              const char* lightPositionName )
{
    LightWorld* world = new LightWorld;
    memset(world, 0, sizeof(LightWorld));
    InitReferenceCounter(&world->refCounter);
    CopyString(lightCountUniformName,
               world->lightCountUniformName,
               sizeof(world->lightCountUniformName));
    CopyString(lightPositionName,
               world->lightPositionName,
               sizeof(world->lightPositionName));
    world->shaderVariableSet = CreateShaderVariableSet();
    world->unusedLightShaderVariableSet = CreateShaderVariableSet();
    return world;
}

static void FreeLightWorld( LightWorld* world )
{
    REPEAT(MAX_LIGHTS, i)
    {
        Light* light = &world->lights[i];
        if(light->active)
        {
            FatalError("Light #%d (%p) was still active when the world was destroyed.",
                       i, light);
            FreeLight(light);
        }
    }
    FreeShaderVariableSet(world->shaderVariableSet);
    FreeShaderVariableSet(world->unusedLightShaderVariableSet);
    delete world;
}

void ReferenceLightWorld( LightWorld* world )
{
    Reference(&world->refCounter);
}

void ReleaseLightWorld( LightWorld* world )
{
    Release(&world->refCounter);
    if(!HasReferences(&world->refCounter))
        FreeLightWorld(world);
}

void SetMaxActiveLightCount( LightWorld* world, int count )
{
    world->maxActiveLightCount = count;
}

static Mat4 CalculateLightTransformation( const Light* light )
{
    const Mat4 solidTransformation =
        TryToGetSolidTransformation(light->attachmentTarget,
                                    light->attachmentFlags);
    return MulMat4(solidTransformation, light->transformation);
}

void UpdateLights( LightWorld* world )
{
    ProfileFunction();
    REPEAT(MAX_LIGHTS, i)
    {
        Light* light = &world->lights[i];
        if(!light->active || light->type == GLOBAL_LIGHT)
            continue;

        const Mat4 transformation = CalculateLightTransformation(light);

        light->position = MulMat4ByVec3(transformation, Vec3Zero);
    }
}

ShaderVariableSet* GetLightWorldShaderVariableSet( const LightWorld* world )
{
    return world->shaderVariableSet;
}

ShaderVariableSet* GetLightWorldUnusedLightShaderVariableSet( const LightWorld* world )
{
    return world->unusedLightShaderVariableSet;
}

static float CalcLightIlluminance( const Light* light,
                                   Vec3 objectPosition,
                                   float objectRadius )
{
    switch(light->type)
    {
        case GLOBAL_LIGHT:
            return light->value;

        case POINT_LIGHT:
            Vec3 delta = {{objectPosition._[0] - light->position._[0],
                           objectPosition._[1] - light->position._[1],
                           objectPosition._[2] - light->position._[2]}};

            float distance = Vec3Length(delta) - objectRadius - light->range;
            if(distance < 0)
                distance = 0;

            const float denominator = 4 * PI * distance * distance;
            return light->value / denominator; // I = P / (4 PI r^2)
    }
    FatalError("Unknown light type.");
    return 0;
}

void GenerateLightShaderVariables( const LightWorld* world,
                                   ShaderVariableSet* variableSet,
                                   Vec3 objectPosition,
                                   float objectRadius )
{
    const int maxCount = world->maxActiveLightCount;
    int count = 0;
    ActiveLight* activeLights = new ActiveLight[maxCount];

    ActiveLight* leastImportantActiveLight = NULL;

    // Populate activeLights:
    REPEAT(MAX_LIGHTS, i)
    {
        const Light* light = &world->lights[i];
        if(!light->active)
            continue;

        float illuminance = CalcLightIlluminance(light,
                                                 objectPosition,
                                                 objectRadius);
        // Ignore lights which doesn't illuminate the object:
        if(illuminance > 0)
        {
            // Fill activeLights array:
            if(count < maxCount)
            {
                activeLights[count].light = light;
                activeLights[count].illuminance = illuminance;
                if(!leastImportantActiveLight ||
                   leastImportantActiveLight->illuminance < illuminance)
                {
                    leastImportantActiveLight = &activeLights[count];
                }
                count++;
            }
            // When activeLights is full and current light has a higher
            // illuminance: Replace active light with lowest illuminance!
            else if(illuminance > leastImportantActiveLight->illuminance)
            {
                leastImportantActiveLight->light = light;
                leastImportantActiveLight->illuminance = illuminance;

                // Find active light with lowest illuminance:
                leastImportantActiveLight = &activeLights[0];
                for(int j = 1; j < maxCount; j++)
                {
                    if(activeLights[j].illuminance <
                       leastImportantActiveLight->illuminance)
                    {
                        leastImportantActiveLight = &activeLights[j];
                    }
                }
            }
        }
    }


    // Upload lights to shader program:
    SetIntUniform(variableSet, world->lightCountUniformName, count);
    REPEAT(count, i)
    {
        const Light* light = activeLights[i].light;

        // Position:
        const char* positionName = Format("%s[%d]", world->lightPositionName, i);

        if(light->type == GLOBAL_LIGHT)
            SetUnusedShaderVariable(variableSet, positionName);
        else
            SetVec3Uniform(variableSet, positionName, light->position);

        // Other uniforms:
        CopyShaderVariablesAsArrayElements(variableSet, light->shaderVariableSet, i);
    }
    for(int i = count; i < maxCount; i++)
    {
        // Use the 'unused light' variable set for ... well ... unused lights:
        CopyShaderVariablesAsArrayElements(variableSet, world->unusedLightShaderVariableSet, i);
    }

    delete[] activeLights;
}

static Light* FindInactiveLight( LightWorld* world )
{
    for(int i = 0; i < MAX_LIGHTS; i++)
    {
        Light* light = &world->lights[i];
        if(!light->active)
            return light;
    }
    return NULL;
}

Light* CreateLight( LightWorld* world, LightType type )
{
    Light* light = FindInactiveLight(world);
    if(!light)
        FatalError("Can't create more lights.");

    memset(light, 0, sizeof(Light));
    light->active = true;
    light->type = type;
    InitReferenceCounter(&light->refCounter);
    light->shaderVariableSet = CreateShaderVariableSet();
    light->transformation = Mat4Identity;
    return light;
}

static void FreeLight( Light* light )
{
    light->active = false;
    FreeReferenceCounter(&light->refCounter);
    if(light->attachmentTarget)
        ReleaseSolid(light->attachmentTarget);
    FreeShaderVariableSet(light->shaderVariableSet);
}

void ReferenceLight( Light* light )
{
    Reference(&light->refCounter);
}

void ReleaseLight( Light* light )
{
    Release(&light->refCounter);
    if(!HasReferences(&light->refCounter))
        FreeLight(light);
}

void SetLightAttachmentTarget( Light* light, Solid* target, int flags )
{
    if(light->attachmentTarget)
        ReleaseSolid(light->attachmentTarget);
    light->attachmentTarget = target;
    light->attachmentFlags = flags;
    if(light->attachmentTarget)
        ReferenceSolid(light->attachmentTarget);
}

void SetLightTransformation( Light* light, Mat4 transformation )
{
    if(light->type == GLOBAL_LIGHT)
        FatalError("Global lights have no transformation.");
    else
        light->transformation = transformation;
}

void SetLightValue( Light* light, float value )
{
    light->value = value;
}

void SetLightRange( Light* light, float range )
{
    if(light->type == GLOBAL_LIGHT)
        FatalError("Global lights have no light range.");
    else
        light->range = range;
}

ShaderVariableSet* GetLightShaderVariableSet( const Light* light )
{
    return light->shaderVariableSet;
}
