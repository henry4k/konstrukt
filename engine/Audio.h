#ifndef __APOAPSIS_AUDIO__
#define __APOAPSIS_AUDIO__

#include "Math.h"


struct Solid;


struct AudioBuffer;
struct AudioSource;


bool InitAudio();
void DestroyAudio();
void UpdateAudio();

void SetAudioGain( float gain );
void SetAudioListenerAttachmentTarget( Solid* target );
void SetAudioListenerTransformation( glm::mat4 transformation );

AudioBuffer* LoadAudioBuffer( const char* fileName );
void ReferenceAudioBuffer( AudioBuffer* buffer );
void ReleaseAudioBuffer( AudioBuffer* buffer );

AudioSource* CreateAudioSource();
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


#endif
