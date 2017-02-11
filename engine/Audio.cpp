#include <string.h>

#if defined(__APPLE__)
    #include <al.h>
    #include <alc.h>
    //#include <alext.h>
    #include <OpenAL/alure.h>
#else
    #include <AL/al.h>
    #include <AL/alc.h>
    #include <AL/alure.h>
#endif

#include "Common.h"
#include "Config.h"
#include "Reference.h"
#include "PhysicsManager.h"
#include "Audio.h"


struct AudioBuffer
{
    ReferenceCounter refCounter;
    ALuint handle;
};

struct AudioSource
{
    ReferenceCounter refCounter;
    ALuint handle;
    bool active;
    Solid* attachmentTarget;
    int attachmentFlags;
    Mat4 transformation;
};


static bool IsActiveAudioSource( AudioSource* source );
static void UpdateAudioListener();
static void FreeAudioSource( AudioSource* source );
static void UpdateAudioSource( AudioSource* source );
static const char* GetALErrorString();
static void CheckALError( const char* origin );
static void PrintAudioDevices();

static AudioSource* AudioSources = NULL;
static int AudioSourceCount = 0;
static float MaxAudioSourceDistance = 0;
static float AudioSourceReferenceDistance = 0;
static Solid* ListenerAttachmentTarget = NULL;
static int ListenerAttachmentFlags = 0;
static Mat4 ListenerTransformation = Mat4Identity;


// --- Global ---

void InitAudio()
{
    if(GetConfigBool("audio.print-devices", false))
        PrintAudioDevices();

    const char* deviceName = GetConfigString("audio.device", NULL);
    if(deviceName)
        Log("Using audio device: ", deviceName);
    else
        Log("Using default audio device.");

    if(!alureInitDevice(deviceName, NULL))
        FatalError("Failed to initialize audio device: %s", alureGetErrorString());

    ALuint alureMajor = 0;
    ALuint alureMinor = 0;
    alureGetVersion(&alureMajor, &alureMinor);

    Log("Using OpenAL %s\n"
        "Vendor: %s\n"
        "Renderer: %s\n"
        "Alure: %d.%d",
        alGetString(AL_VERSION),
        alGetString(AL_VENDOR),
        alGetString(AL_RENDERER),
        alureMajor, alureMinor);

    AudioSourceCount = GetConfigInt("audio.max-sources", 32);
    AudioSources = new AudioSource[AudioSourceCount];
    memset(AudioSources, 0, sizeof(AudioSource)*AudioSourceCount);

    MaxAudioSourceDistance = GetConfigFloat("audio.max-distance", 100);
    AudioSourceReferenceDistance = GetConfigFloat("audio.reference-distance", 100);

    ListenerAttachmentTarget = NULL;

    SetAudioGain(GetConfigFloat("audio.gain", 1.0f));

    // TODO: Set distance model, doppler factor and so on!
    alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED); // ?
    //alDopplerFactor();
    //alDopplerVelocity();
}

void DestroyAudio()
{
    SetAudioListenerAttachmentTarget(NULL, 0);

    for(int i = 0; i < AudioSourceCount; ++i)
        if(IsActiveAudioSource(&AudioSources[i]))
            FreeAudioSource(&AudioSources[i]);
    delete[] AudioSources;

    alureShutdownDevice();
}

void UpdateAudio()
{
    UpdateAudioListener();

    for(int i = 0; i < AudioSourceCount; ++i)
        if(IsActiveAudioSource(&AudioSources[i]))
            UpdateAudioSource(&AudioSources[i]);
}


// --- Listener ---

void SetAudioGain( float gain )
{
    alListenerf(AL_GAIN, gain);
}

void SetAudioListenerAttachmentTarget( Solid* target, int flags )
{
    if(ListenerAttachmentTarget)
        ReleaseSolid(ListenerAttachmentTarget);
    ListenerAttachmentTarget = target;
    ListenerAttachmentFlags = flags;
    if(ListenerAttachmentTarget)
        ReferenceSolid(ListenerAttachmentTarget);
}

void SetAudioListenerTransformation( Mat4 transformation )
{
    ListenerTransformation = transformation;
}

const Vec3 Forward = {{0,0,1}};
const Vec3 Up      = {{0,1,0}};

static void UpdateAudioListener()
{
    const Mat4 targetTransformation =
        TryToGetSolidTransformation(ListenerAttachmentTarget,
                                    ListenerAttachmentFlags);

    const Mat4 finalTransformation = MulMat4(targetTransformation,
                                             ListenerTransformation);
    const Vec3 position  = MulMat4ByVec3(finalTransformation, Vec3Zero);

    const Mat4 rotation = ClipTranslationOfMat4(finalTransformation);
    const Vec3 direction = MulMat4ByVec3(rotation, Forward);
    const Vec3 up        = MulMat4ByVec3(rotation, Up);

    Vec3 velocity = Vec3Zero;
    if(ListenerAttachmentTarget)
        velocity = GetSolidLinearVelocity(ListenerAttachmentTarget);

    float orientation[6];
    memcpy(orientation,   direction._, sizeof(float)*3);
    memcpy(orientation+3, up._,        sizeof(float)*3);

    alListenerfv(AL_POSITION, position._);
    alListenerfv(AL_VELOCITY, velocity._);
    alListenerfv(AL_ORIENTATION, orientation);
}


// --- AudioBuffer ---

AudioBuffer* LoadAudioBuffer( const char* fileName )
{
    const ALuint handle = alureCreateBufferFromFile(fileName);
    if(handle == AL_NONE)
        FatalError("Can't load %s: %s", fileName, alureGetErrorString());
    CheckALError("LoadAudioBuffer");

    Log("Loaded %s", fileName);
    AudioBuffer* buffer = new AudioBuffer;
    memset(buffer, 0, sizeof(AudioBuffer));
    buffer->handle = handle;
    return buffer;
}

static void FreeAudioBuffer( AudioBuffer* buffer )
{
    alDeleteBuffers(1, &buffer->handle);
    CheckALError("FreeAudioBuffer");
    delete buffer;
}

void ReferenceAudioBuffer( AudioBuffer* buffer )
{
    Reference(&buffer->refCounter);
}

void ReleaseAudioBuffer( AudioBuffer* buffer )
{
    Release(&buffer->refCounter);
    if(!HasReferences(&buffer->refCounter))
        FreeAudioBuffer(buffer);
}


// --- AudioSource ---

static bool IsActiveAudioSource( AudioSource* source )
{
    return source->handle != AL_NONE;
}

static AudioSource* FindFreeAudioSource()
{
    for(int i = 0; i < AudioSourceCount; ++i)
        if(!IsActiveAudioSource(&AudioSources[i]))
            return &AudioSources[i];
    return NULL;
}

AudioSource* CreateAudioSource()
{
    AudioSource* source = FindFreeAudioSource();
    if(!source)
        FatalError("Reached audio source limit! (%d sources)", AudioSourceCount);

    ALuint handle = AL_NONE;
    alGenSources(1, &handle);

    memset(source, 0, sizeof(AudioSource));
    source->handle = handle;
    source->transformation = Mat4Identity;

    alSourcef(handle, AL_MAX_DISTANCE, MaxAudioSourceDistance);
    alSourcef(handle, AL_REFERENCE_DISTANCE, AudioSourceReferenceDistance);

    return source;
}

static void FreeAudioSource( AudioSource* source )
{
    alDeleteSources(1, &source->handle);
    source->handle = AL_NONE;
    CheckALError("FreeAudioSourceAtIndex");
}

void ReferenceAudioSource( AudioSource* source )
{
    Reference(&source->refCounter);
}

void ReleaseAudioSource( AudioSource* source )
{
    Release(&source->refCounter);
    if(!HasReferences(&source->refCounter))
        FreeAudioSource(source);
}

void SetAudioSourceRelative( AudioSource* source, bool relative )
{
    alSourcei(source->handle, AL_SOURCE_RELATIVE, (relative ? AL_TRUE : AL_FALSE));
    CheckALError("SetAudioSourceRelative");
}

void SetAudioSourceLooping( AudioSource* source, bool loop )
{
    alSourcei(source->handle, AL_LOOPING, (loop ? AL_TRUE : AL_FALSE));
    CheckALError("SetAudioSourceLooping");
}

void SetAudioSourcePitch( AudioSource* source, float pitch )
{
    alSourcef(source->handle, AL_PITCH, pitch);
    CheckALError("SetAudioSourcePitch");
}

void SetAudioSourceGain( AudioSource* source, float gain )
{
    alSourcef(source->handle, AL_GAIN, gain);
    CheckALError("SetAudioSourceGain");
}

void SetAudioSourceAttachmentTarget( AudioSource* source, Solid* target, int flags )
{
    if(source->attachmentTarget)
        ReleaseSolid(source->attachmentTarget);
    source->attachmentTarget = target;
    source->attachmentFlags = flags;
    if(source->attachmentTarget)
        ReferenceSolid(source->attachmentTarget);
}

void SetAudioSourceTransformation( AudioSource* source, Mat4 transformation )
{
    source->transformation = transformation;
}

static void UpdateAudioSource( AudioSource* source )
{
    int state = 0;
    alGetSourcei(source->handle, AL_SOURCE_STATE, &state);
    if(state == AL_PLAYING)
    {
        Mat4 targetTransformation =
            TryToGetSolidTransformation(source->attachmentTarget,
                                        source->attachmentFlags);
        const Mat4 finalTransformation = MulMat4(targetTransformation,
                                                 source->transformation);

        const Vec3 position = MulMat4ByVec3(finalTransformation, Vec3Zero);
        alSourcefv(source->handle, AL_POSITION, position._);

        if(source->attachmentTarget)
        {
            const Vec3 velocity = GetSolidLinearVelocity(source->attachmentTarget);

            const Mat4 rotation  = ClipTranslationOfMat4(finalTransformation);
            const Vec3 direction = MulMat4ByVec3(rotation, Vec3Zero);

            alSourcefv(source->handle, AL_VELOCITY,  velocity._);
            alSourcefv(source->handle, AL_DIRECTION, direction._);
        }

        if(!source->active)
        {
            ReferenceAudioSource(source);
            source->active = true;
            Log("Source %p activated!", source);
        }
    }
    else
    {
        if(source->active)
        {
            ReleaseAudioSource(source);
            source->active = false;
            Log("Source %p deactivated!", source);
        }
    }

    CheckALError("UpdateAudioSource");
}

void EnqueueAudioBuffer( AudioSource* source, AudioBuffer* buffer )
{
    alSourceQueueBuffers(source->handle, 1, &buffer->handle);
    CheckALError("EnqueueAudioBuffer");
}

void PlayAudioSource( AudioSource* source )
{
    alSourcePlay(source->handle);
    CheckALError("PlayAudioSource");
}

void PauseAudioSource( AudioSource* source )
{
    alSourcePause(source->handle);
    CheckALError("PauseAudioSource");
}


// --- Utils ---

static const char* GetALErrorString()
{
    const ALenum error = alGetError();
    if(error != AL_NO_ERROR)
    {
        switch(error)
        {
            case AL_INVALID_NAME:      return "Invalid name";
            case AL_INVALID_ENUM:      return "Invalid enum";
            case AL_INVALID_VALUE:     return "Invalid value";
            case AL_INVALID_OPERATION: return "Invalid operation";
            case AL_OUT_OF_MEMORY:     return "Out of memory";
            default:                   return "Unknown issue";
        }
    }

    return NULL;
}

static void CheckALError( const char* origin )
{
    const char* error = GetALErrorString();
    if(error)
        FatalError("%s: %s", origin, error);
}

static void PrintAudioDevices()
{
    ALCsizei deviceCount = 0;
    const char** deviceNames = alureGetDeviceNames(true, &deviceCount);
    if(!deviceNames || deviceCount == 0)
        FatalError("Failed to query audio device list: %s", alureGetErrorString());

    Log("Available audio devices:");
    for(int i = 0; i < deviceCount; ++i)
        Log("  %s", deviceNames[i]);

    alureFreeDeviceNames(deviceNames);
}
