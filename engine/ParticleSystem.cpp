#include <assert.h>
#include <string.h> // memset
#include "Common.h"
#include "OpenGL.h" // glfwGetTime
#include "Math.h"
#include "Mesh.h"
#include "Reference.h"
#include "ParticleSystem.h"


static const int MAX_PARTICLE_SYSTEMS = 8;


struct Particle
{
    float birthTime;
    glm::vec3 position;
    glm::vec3 velocity;
};

struct ParticleSystem
{
    bool active;
    ReferenceCounter refCounter;

    int maxParticleCount;
    int activeParticleCount;
    Particle* particles;

    float maxParticleLifeTime;

    Mesh* mesh;
};


static ParticleSystem ParticleSystems[MAX_PARTICLE_SYSTEMS];


static void FreeParticleSystem( ParticleSystem* system );
static void UpdateParticleSystem( ParticleSystem* system, double timeDelta );

bool InitParticleSystemManager()
{
    memset(ParticleSystems, 0, sizeof(ParticleSystems));
    return true;
}

void DestroyParticleSystemManager()
{
    for(int i = 0; i < MAX_PARTICLE_SYSTEMS; i++)
    {
        ParticleSystem* system = &ParticleSystems[i];
        if(system->active)
        {
            Error("Particle system #%d (%p) was still active when the manager was destroyed.",
                  i, system);
            FreeParticleSystem(system);
        }
    }
}

void UpdateParticleSystems( double timeDelta )
{
    for(int i = 0; i < MAX_PARTICLE_SYSTEMS; i++)
    {
        ParticleSystem* system = &ParticleSystems[i];
        if(system->active)
            UpdateParticleSystem(system, timeDelta);
    }
}

static ParticleSystem* FindFreeParticleSystem()
{
    for(int i = 0; i < MAX_PARTICLE_SYSTEMS; i++)
    {
        ParticleSystem* system = &ParticleSystems[i];
        if(!system->active)
            return system;
    }
    return NULL;
}

ParticleSystem* CreateParticleSystem( int maxParticleCount,
                                      float maxParticleLifeTime )
{
    ParticleSystem* system = FindFreeParticleSystem();
    if(!system)
    {
        Error("Can't create more particle systems.");
        return NULL;
    }

    memset(system, 0, sizeof(ParticleSystem));

    system->active = true;
    InitReferenceCounter(&system->refCounter);

    system->maxParticleCount = maxParticleCount;
    system->particles = new Particle[maxParticleCount];
    memset(system->particles, 0, sizeof(Particle)*maxParticleCount);

    system->maxParticleLifeTime = maxParticleLifeTime;

    // system->mesh = CreateMesh(); // TODO
    // ReferenceMesh(system->mesh);

    return system;
}

static void FreeParticleSystem( ParticleSystem* system )
{
    system->active = false;
    FreeReferenceCounter(&system->refCounter);
    delete[] system->particles;
    // ReleaseMesh(system->mesh);
}

static void AddParticle( ParticleSystem* system,
                         glm::vec3 position,
                         glm::vec3 velocity )
{
    const int maxParticleCount = system->maxParticleCount;
    const int activeParticleCount = system->activeParticleCount;
    Particle* particles = system->particles;

    if(activeParticleCount < maxParticleCount)
    {
        const int index = activeParticleCount;
        Particle* particle = &particles[index];

        particle->birthTime = glfwGetTime();
        particle->position = position;
        particle->velocity = velocity;

        system->activeParticleCount++;
    }
}

static void RemoveParticle( ParticleSystem* system, int index )
{
    const int activeParticleCount = system->activeParticleCount;
    Particle* particles = system->particles;

    assert(activeParticleCount > 0 &&
           index >= 0 &&
           index < activeParticleCount);

    particles[index] = particles[activeParticleCount-1];

    system->activeParticleCount--;
}

static void UpdateParticleSystem( ParticleSystem* system, double timeDelta )
{
    const int activeParticleCount = system->activeParticleCount;
    Particle* particles = system->particles;
    const float maxParticleLifeTime = system->maxParticleLifeTime;

    const float currentTime = glfwGetTime();

    int index = 0;
    while(index < activeParticleCount)
    {
        Particle* particle = &particles[index];
        const float particleLifeTime = currentTime - particle->birthTime;
        if(particleLifeTime < maxParticleLifeTime)
        {
            particle->position += particle->velocity * (float)timeDelta;
            // Additional simulation stuff goes here.
        }
        else
        {
            RemoveParticle(system, index);
        }
        index++;
    }
}

void SpawnParticle( ParticleSystem* system,
                    glm::vec3 position,
                    glm::vec3 velocity )
{
    AddParticle(system, position, velocity);
}

Mesh* GetParticleSystemMesh( ParticleSystem* system )
{
    return system->mesh;
}

void ReferenceParticleSystem( ParticleSystem* system )
{
    Reference(&system->refCounter);
}

void ReleaseParticleSystem( ParticleSystem* system )
{
    Release(&system->refCounter);
    if(!HasReferences(&system->refCounter))
        FreeParticleSystem(system);
}
