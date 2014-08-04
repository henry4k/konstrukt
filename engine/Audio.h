#ifndef __APOAPSIS_AUDIO__
#define __APOAPSIS_AUDIO__

#include "Math.h"


struct Solid;


struct AudioBuffer;
struct AudioSource;


/**
 * Keep in mind that this function may or may not be called, depending on
 * how the sound is stopped. E.g. DestroyAudio() forcefully deletes all
 * sounds and doesn't care about stop functions.
 */
typedef void (*AudioSourceStopFn)( AudioSource* source, void* context );

void FreeAudioSourceOnStop( AudioSource source, void* context );


bool InitAudio();
void DestroyAudio();
void UpdateAudio();

void SetAudioGain( float gain );
void SetAudioListenerAttachmentTarget( Solid* target );
void SetAudioListenerTransformation( glm::mat4 transformation );

AudioBuffer* LoadAudioBuffer( const char* fileName );
void ReferenceAudioBuffer( AudioBuffer* buffer );
void ReleaseAudioBuffer( AudioBuffer* buffer );

/**
 * Tries to create a new audio source.
 *
 * @param stopFn
 *      Function that is called when the source stopps playing.
 *      If the parameter is `NULL`, nothing will be done.
 *
 * @param context
 *      Only relevant if stopFn is given.
 *
 */
AudioSource* CreateAudioSource( AudioSourceStopFn stopFn, void* context );
void ReferenceAudioSource( AudioSource* source );
void ReleaseAudioSource( AudioSource* source );

void SetAudioSourceRelative( AudioSource* source, bool relative );
void SetAudioSourceLooping( AudioSource* source, bool loop );
void SetAudioSourcePitch( AudioSource* source, float pitch );
void SetAudioSourceGain( AudioSource* source, float gain );
void SetAudioSourceAttachmentTarget( AudioSource* source, Solid* target );
void SetAudioSourceTransformation( AudioSource* source, glm::mat4 transformation );

void EnqueueAudioBuffer( AudioSource* source, AudioBuffer* buffer );
void PlayAudioSource( AudioSource* source );
void PauseAudioSource( AudioSource* source );

void PlayAllAudioSources();
void PauseAllAudioSources();

#endif
