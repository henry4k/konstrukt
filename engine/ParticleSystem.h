#ifndef __APOAPSIS_PARTICLE_SYSTEM__
#define __APOAPSIS_PARTICLE_SYSTEM__

#include "Math.h"


struct Mesh;

/**
 * Particle systems hold and simulate their particles.
 *
 * Rendering is not done by a particle system itself, instead it just
 * controls a #Mesh.  Which can then be rendered using a #Model.
 *
 * Particles have a quite fixed feature set, to ensure performance and a
 * simple implementation.
 */
struct ParticleSystem;


bool InitParticleSystemModule();
void DestroyParticleSystemModule();
void UpdateParticleSystems( double timeDelta );

ParticleSystem* CreateParticleSystem();
Mesh* GetParticleSystemMesh( ParticleSystem* system );

void SetParticleLifetime( ParticleSystem* system, float lifeTime );

void SpawnParticle( ParticleSystem* system,
                    glm::vec3 position,
                    glm::vec3 velocity );

void ReferenceParticleSystem( ParticleSystem* system );
void ReleaseParticleSystem( ParticleSystem* system );


#endif
