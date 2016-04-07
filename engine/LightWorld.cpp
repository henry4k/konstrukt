#include <string.h> // memset, strcmp
#include <stdlib.h> // qsort

#include "Common.h"
#include "Reference.h"
#include "PhysicsManager.h"
#include "LightWorld.h"


static const int MAX_LIGHTS = 32;
static const int MAX_LIGHT_UNIFORMS = 8;


struct LightUniform
{
    char name[MAX_UNIFORM_NAME_SIZE];
    UniformType type;
    UniformValue value;
};

struct Light
{
    bool active;
    ReferenceCounter refCounter;
    LightUniform uniforms[MAX_LIGHT_UNIFORMS];
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
    return world;
}

static void FreeLightWorld( LightWorld* world )
{
    REPEAT(MAX_LIGHTS, i)
    {
        Light* light = &world->lights[i];
        if(light->active)
        {
            Error("Light #%d (%p) was still active when the world was destroyed.",
                  i, light);
            FreeLight(light);
        }
    }
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
    REPEAT(MAX_LIGHTS, i)
    {
        Light* light = &world->lights[i];
        if(!light->active)
            continue;

        const Mat4 transformation = CalculateLightTransformation(light);

        light->position = MulMat4ByVec3(transformation, Vec3Zero);
    }
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
            // TODO
            return 0;
    }
    FatalError("Unknown light type.");
    return 0;
}

void SetLightUniforms( LightWorld* world,
                       ShaderProgram* program,
                       Vec3 objectPosition,
                       float objectRadius )
{
    const int maxCount = world->maxActiveLightCount;
    int count = 0;
    ActiveLight* activeLights = new ActiveLight[maxCount]; // TODO: Optimize

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

    UniformValue countValue;
    countValue.i = count;
    SetUniform(program, world->lightCountUniformName, INT_UNIFORM, &countValue);

    REPEAT(count, i)
    {
        const Light* light = activeLights[i].light;

        REPEAT(MAX_LIGHT_UNIFORMS, j)
        {
            // Position:
            const char* positionName = Format(world->lightPositionName, i);
            UniformValue positionValue;
            positionValue.vec3 = light->position;
            SetUniform(program, positionName, VEC3_UNIFORM, &positionValue);

            // Other uniforms:
            const LightUniform* uniform = &light->uniforms[j];
            if(uniform->name[0] != '\0')
            {
                const char* name = Format(uniform->name, i);
                SetUniform(program, name, uniform->type, &uniform->value);
            }
        }
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
    if(light)
    {
        memset(light, 0, sizeof(Light));
        light->active = true;
        light->type = type;
        InitReferenceCounter(&light->refCounter);
        return light;
    }
    else
    {
        Error("Can't create more lights.");
        return NULL;
    }
}

static void FreeLight( Light* light )
{
    light->active = false;
    FreeReferenceCounter(&light->refCounter);
    if(light->attachmentTarget)
        ReleaseSolid(light->attachmentTarget);
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
    light->transformation = transformation;
}

void SetLightValue( Light* light, float value )
{
    light->value = value;
}

void SetLightRange( Light* light, float range )
{
    light->range = range;
}

static LightUniform* FindUniform( Light* light, const char* name )
{
    LightUniform* uniforms = light->uniforms;
    REPEAT(MAX_LIGHT_UNIFORMS, i)
        if(strncmp(name, uniforms[i].name, MAX_UNIFORM_NAME_SIZE-1) == 0)
            return &uniforms[i];
    return NULL;
}

static LightUniform* FindFreeUniform( Light* light )
{
    return FindUniform(light, "");
}

void SetLightUniform( Light* light,
                      const char* name,
                      UniformType type,
                      const UniformValue* value )
{
    LightUniform* uniform = FindUniform(light, name);
    if(!uniform)
        uniform = FindFreeUniform(light);
    if(!uniform)
        FatalError("Too many uniforms for light %p.", light);

    memset(uniform, 0, sizeof(LightUniform));

    CopyString(name, uniform->name, sizeof(uniform->name));
    uniform->type = type;
    memcpy(&uniform->value, value, GetUniformSize(type));
}

void UnsetLightUniform( Light* light, const char* name )
{
    LightUniform* uniform = FindUniform(light, name);
    if(uniform)
        uniform->name[0] = '\0';
}
