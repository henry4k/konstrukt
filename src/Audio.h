#ifndef __AUDIO__
#define __AUDIO__

#include "Math.h"

typedef unsigned int AudioBuffer;
typedef unsigned int AudioSource;

/**
 * Keep in mind that this function may or may not be called,
 * depending on how the sound is stopped.
 * E.g. DestroyAudio() forcefully deletes all sounds and doen't care about stop functions.
 */
typedef void (*AudioSourceStopFn)( AudioSource source, void* context );

void FreeAudioSourceOnStop( AudioSource source, void* context );


bool InitAudio();
void DestroyAudio();
void UpdateAudio( float timeDelta );

void SetAudioGain( float gain );

/**
 * Sets the listeners properties. (i.e. where your ears are in the virtual world)
 *
 * @param position
 *      3D vector
 *
 * @param velocity
 *      3D vector
 *
 * @param direction
 *      3D vector
 *
 * @param up
 *      3D vector
 */
void UpdateAudioListener( glm::vec3 position, glm::vec3 velocity, glm::vec3 direction, glm::vec3 up );

AudioBuffer LoadAudioBuffer( const char* fileName );
void FreeAudioBuffer( AudioBuffer buffer );

/**
 * Tries to create a new audio source.
 *
 * @param stopFn
 *		Function that is called when the source stopps playing.
 *		If the parameter is `NULL`, nothing will be done.
 *
 * @param context
 *		Only relevant if stopFn is given.
 *		
 */
AudioSource CreateAudioSource( AudioSourceStopFn stopFn, void* context );
void FreeAudioSource( AudioSource source );

void SetAudioSourceRelative( AudioSource source, bool relative );
void SetAudioSourceLooping( AudioSource source, bool loop );
void SetAudioSourcePitch( AudioSource source, float pitch );
void SetAudioSourceGain( AudioSource source, float gain );

void EnqueueAudioBuffer( AudioSource source, AudioBuffer buffer );
void PlayAudioSource( AudioSource source );
void PauseAudioSource( AudioSource source );

/**
 * Sets source properties.
 *
 * @param position
 *      3D vector
 *
 * @param velocity
 *      3D vector
 *
 * @param direction
 *      3D vector
 */
void UpdateAudioSource( AudioSource source, glm::vec3 position, glm::vec3 velocity, glm::vec3 direction );

#endif
