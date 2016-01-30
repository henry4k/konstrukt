#ifndef __KONSTRUKT_AUDIO__
#define __KONSTRUKT_AUDIO__

#include "Math.h"


struct Solid;


/** @class AudioListener
 * Describes the properties of your virtual ears.
 *
 * Like the #AudioSource the listener has a transformation and an attachment
 * target. The attachment target provides information about position,
 * orientation and velocity. If the listener has no attachment target, those
 * properties are zero. The transformation matrix is applied additionaly after
 * the attachment targets position has been applied.
 *
 * @see SetAudioListenerAttachmentTarget
 * @see SetAudioListenerTransformation
 */

/**
 * An audio buffer contains audio data, which is e.g. loaded from a file.
 *
 * Audio buffers are reference counted. You need to call #ReferenceAudioBuffer
 * whenever you store its handle somewhere and call #ReleaseAudioBuffer once
 * you don't need it anymore.
 */
struct AudioBuffer;

/**
 * Can play positional audio in an 3D environment.
 *
 * Like the #AudioListener, the audio source has a transformation and an
 * attachment target. The attachment target provides information about position,
 * direction and velocity. If the audio source has no attachment target, those
 * properties are zero. The transformation matrix is applied additionaly after
 * the attachment targets position has been applied.
 *
 * Audio sources are reference counted. You need to call #ReferenceAudioSource
 * whenever you store its handle somewhere and call #ReleaseAudioSource once
 * you don't need it anymore.
 */
struct AudioSource;


bool InitAudio();
void DestroyAudio();

/**
 * Needs to be called regularly, e.g. once per frame.
 */
void UpdateAudio();

/**
 * Set global audio volume.
 */
void SetAudioGain( float gain );


/**
 * Attaches the listener to a solid.
 *
 * Passing `NULL` detaches the listener; this has the same effect as if it the
 * solid has been positioned at 0,0,0.
 */
void SetAudioListenerAttachmentTarget( Solid* target, int flags );

void SetAudioListenerTransformation( Mat4 transformation );


/**
 * Initializes an audio buffer with wave data from a sound file.
 *
 * @return The audio buffer or `NULL` if loading failed.
 */
AudioBuffer* LoadAudioBuffer( const char* fileName );

void ReferenceAudioBuffer( AudioBuffer* buffer );

void ReleaseAudioBuffer( AudioBuffer* buffer );



AudioSource* CreateAudioSource();

void ReferenceAudioSource( AudioSource* source );

void ReleaseAudioSource( AudioSource* source );

/**
 * If set, the audio sources position is relative to the listeners position.
 *
 * I.e. if you position it at (1,0,0) the sound will always be on the listeners
 * right side. No matter how the listener is positioned.
 */
void SetAudioSourceRelative( AudioSource* source, bool relative );

/**
 * If set, the audio source will start from the beginning when the last enqueued
 * buffer has been played.
 */
void SetAudioSourceLooping( AudioSource* source, bool loop );

void SetAudioSourcePitch( AudioSource* source, float pitch );

void SetAudioSourceGain( AudioSource* source, float gain );

void SetAudioSourceAttachmentTarget( AudioSource* source, Solid* target, int flags );

void SetAudioSourceTransformation( AudioSource* source, Mat4 transformation );

// TODO: Should store references correctly? Or does that OpenAL already?
void EnqueueAudioBuffer( AudioSource* source, AudioBuffer* buffer );

/**
 * Starts or continues to play the enqueued buffers.
 */
void PlayAudioSource( AudioSource* source );

void PauseAudioSource( AudioSource* source );


#endif
