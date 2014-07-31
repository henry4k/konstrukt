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
#include "PhysicsManager.h"
#include "Audio.h"

struct AudioSourceInfo
{
    AudioSource handle;
    AudioSourceStopFn stopFn;
    void* context;
    Solid* attachmentTarget;
    glm::mat4 transformation;
};

void FreeAudioSourceOnStop( AudioSource source, void* context )
{
    FreeAudioSource(source);
}

static void FreeAudioSourceAtIndex( int index );
static void UpdateAudioListener();
static void UpdateAudioSource( const AudioSourceInfo* sourceInfo );

static AudioSourceInfo* AudioSources = NULL;
static int AudioSourceCount;
static float MaxAudioSourceDistance;
static float AudioSourceReferenceDistance;
static Solid* ListenerAttachmentTarget = NULL;
static glm::mat4 ListenerTransformation;

static const char* GetALErrorString()
{
    const ALenum error = alGetError();
    if(error != AL_NO_ERROR)
    {
        switch(error)
        {
            case AL_INVALID_NAME: return "Invalid name";
            case AL_INVALID_ENUM: return "Invalid enum";
            case AL_INVALID_VALUE: return "Invalid value";
            case AL_INVALID_OPERATION: return "Invalid operation";
            case AL_OUT_OF_MEMORY: return "Out of memory";
            default: return "Unknown issue";
        }
    }

    return NULL;
}

static bool PrintALError( const char* origin )
{
    const char* error = GetALErrorString();
    if(error)
    {
        Error("%s: %s", origin, error);
        return true;
    }
    else
    {
        return false;
    }
}

static void PrintAudioDevices()
{
    ALCsizei deviceCount = 0;
    const char** deviceNames = alureGetDeviceNames(true, &deviceCount);

    if(!deviceNames || deviceCount == 0)
    {
        Error("Failed to query audio device list: %s", alureGetErrorString());
    }
    else
    {
        Log("Available audio devices:");
        for(int i = 0; i < deviceCount; ++i)
            Log("  %s", deviceNames[i]);
    }

    if(deviceNames)
        alureFreeDeviceNames(deviceNames);
}

bool InitAudio()
{
    if(GetConfigBool("audio.print-devices", false))
        PrintAudioDevices();

    const char* deviceName = GetConfigString("audio.device", NULL);
    if(deviceName)
        Log("Using audio device: ", deviceName);
    else
        Log("Using default audio device.");

    if(!alureInitDevice(deviceName, NULL))
    {
        Error("Failed to initialize audio device%s", alureGetErrorString());
        return false;
    }

    ALuint alureMajor = 0;
    ALuint alureMinor = 0;
    alureGetVersion(&alureMajor, &alureMinor);

    Log(
        "Using OpenAL %s\n"
        "Vendor: %s\n"
        "Renderer: %s\n"
        "Alure: %d.%d",

        alGetString(AL_VERSION),
        alGetString(AL_VENDOR),
        alGetString(AL_RENDERER),
        alureMajor, alureMinor
    );

    AudioSourceCount = GetConfigInt("audio.max-sources", 32);
    AudioSources = new AudioSourceInfo[AudioSourceCount];
    memset(AudioSources, 0, sizeof(AudioSourceInfo)*AudioSourceCount);

    MaxAudioSourceDistance = GetConfigFloat("audio.max-distance", 100);
    AudioSourceReferenceDistance = GetConfigFloat("audio.reference-distance", 100);

    ListenerAttachmentTarget = NULL;

    SetAudioGain(GetConfigFloat("audio.gain", 1.0f));

    // TODO: Set distance model, doppler factor and so on!
    alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED); // ?
    //alDopplerFactor();
    //alDopplerVelocity();

    return true;
}

void DestroyAudio()
{
    SetAudioListenerAttachmentTarget(NULL);

    for(int i = 0; i < AudioSourceCount; ++i)
        if(AudioSources[i].handle)
            alDeleteSources(1, &AudioSources[i].handle);
    delete[] AudioSources;

    alureShutdownDevice();
}

void UpdateAudio()
{
    UpdateAudioListener();

    for(int i = 0; i < AudioSourceCount; ++i)
    {
        const AudioSourceInfo* sourceInfo = &AudioSources[i];
        if(sourceInfo->handle)
            UpdateAudioSource(sourceInfo);
    }
}

void SetAudioGain( float gain )
{
    alListenerf(AL_GAIN, gain);
}

void SetAudioListenerAttachmentTarget( Solid* target )
{
    if(ListenerAttachmentTarget)
        ReleaseSolid(ListenerAttachmentTarget);
    ListenerAttachmentTarget = target;
    if(ListenerAttachmentTarget)
        ReferenceSolid(ListenerAttachmentTarget);
}

void SetAudioListenerTransformation( glm::mat4 transformation )
{
    ListenerTransformation = transformation;
}

static void UpdateAudioListener()
{
    using namespace glm;

    mat4 targetTransformation;
    if(ListenerAttachmentTarget)
        GetSolidTransformation(ListenerAttachmentTarget, &targetTransformation);
    const mat4 finalTransformation = targetTransformation *
                                     ListenerTransformation;

    const vec3 position(finalTransformation * vec4());
    const vec3 velocity;
    const vec3 direction(0,0,1);
    const vec3 up(0,1,0);

    float orientation[6];
    memcpy(orientation,   glm::value_ptr(direction), sizeof(float)*3);
    memcpy(orientation+3, glm::value_ptr(up),        sizeof(float)*3);

    alListenerfv(AL_POSITION, glm::value_ptr(position));
    alListenerfv(AL_VELOCITY, glm::value_ptr(velocity));
    alListenerfv(AL_ORIENTATION, orientation);
}

AudioBuffer LoadAudioBuffer( const char* fileName )
{
    const ALuint buffer = alureCreateBufferFromFile(fileName);
    if(buffer == AL_NONE || PrintALError("LoadAudioBuffer"))
    {
        Error("Can't load %s: %s", fileName, alureGetErrorString());
        return AL_NONE;
    }
    else
    {
        Log("Loaded %s", fileName);
        return (AudioBuffer)buffer;
    }
}

void FreeAudioBuffer( AudioBuffer buffer )
{
    const ALuint b = (ALuint)buffer;
    alDeleteBuffers(1, &b);
    PrintALError("FreeAudioBuffer");
}

AudioSource CreateAudioSource( AudioSourceStopFn stopFn, void* context )
{
    for(int i = 0; i < AudioSourceCount; ++i)
    {
        AudioSourceInfo* sourceInfo = &AudioSources[i];
        if(sourceInfo->handle == AL_NONE)
        {
            ALuint source = AL_NONE;
            alGenSources(1, &source);

            sourceInfo->handle = source;
            sourceInfo->stopFn = stopFn;
            sourceInfo->context = context;

            alSourcef(source, AL_MAX_DISTANCE, MaxAudioSourceDistance);
            alSourcef(source, AL_REFERENCE_DISTANCE, AudioSourceReferenceDistance);

            return (AudioSource)source;
        }
    }
    Error("Reached audio source limit! (%d sources)", AudioSourceCount);
    return 0;
}

void FreeAudioSourceAtIndex( int index )
{
    AudioSourceInfo* sourceInfo = &AudioSources[index];
    alDeleteSources(1, &sourceInfo->handle);
    memset(sourceInfo, 0, sizeof(AudioSourceInfo));
    PrintALError("FreeAudioSourceAtIndex");
}

void FreeAudioSource( AudioSource source )
{
    for(int i = 0; i < AudioSourceCount; ++i)
    {
        if(AudioSources[i].handle == source)
        {
            FreeAudioSourceAtIndex(i);
            return;
        }
    }
    Error("Audio source %d not found!", source);
}

void SetAudioSourceRelative( AudioSource source, bool relative )
{
    alSourcei((ALuint)source, AL_SOURCE_RELATIVE, (relative ? AL_TRUE : AL_FALSE));
    PrintALError("SetAudioSourceRelative");
}

void SetAudioSourceLooping( AudioSource source, bool loop )
{
    alSourcei((ALuint)source, AL_LOOPING, (loop ? AL_TRUE : AL_FALSE));
    PrintALError("SetAudioSourceLooping");
}

void SetAudioSourcePitch( AudioSource source, float pitch )
{
    alSourcef((ALuint)source, AL_PITCH, pitch);
    PrintALError("SetAudioSourcePitch");
}

void SetAudioSourceGain( AudioSource source, float gain )
{
    alSourcef((ALuint)source, AL_GAIN, gain);
    PrintALError("SetAudioSourceGain");
}

void SetAudioSourceAttachmentTarget( AudioSource source, Solid* target )
{
    AudioSourceInfo* sourceInfo = NULL;
    for(int i = 0; i < AudioSourceCount; ++i)
        if(AudioSources[i].handle == source)
            sourceInfo = &AudioSources[i];
    if(!sourceInfo)
        return;

    if(sourceInfo->attachmentTarget)
        ReleaseSolid(sourceInfo->attachmentTarget);
    sourceInfo->attachmentTarget = target;
    if(sourceInfo->attachmentTarget)
        ReferenceSolid(sourceInfo->attachmentTarget);
}

void SetAudioSourceTransformation( AudioSource source, glm::mat4 transformation )
{
    AudioSourceInfo* sourceInfo = NULL;
    for(int i = 0; i < AudioSourceCount; ++i)
        if(AudioSources[i].handle == source)
            sourceInfo = &AudioSources[i];
    if(!sourceInfo)
        return;

    sourceInfo->transformation = transformation;
}

static void UpdateAudioSource( const AudioSourceInfo* sourceInfo )
{
    if(sourceInfo->stopFn)
    {
        int state = 0;
        alGetSourcei(sourceInfo->handle, AL_SOURCE_STATE, &state);
        if(!PrintALError("UpdateAudio") && (state == AL_STOPPED || state == AL_INITIAL)) // TODO: Check enum values
        {
            sourceInfo->stopFn( sourceInfo->handle, sourceInfo->context );
            return;
        }
    }

    using namespace glm;

    mat4 targetTransformation;
    if(sourceInfo->attachmentTarget)
        GetSolidTransformation(sourceInfo->attachmentTarget, &targetTransformation);
    const mat4 finalTransformation = targetTransformation *
                                     sourceInfo->transformation;

    const vec3 position(finalTransformation * vec4());
    const vec3 velocity;
    const vec3 direction(0,0,1);
    const vec3 up(0,1,0);

    const ALuint s = (ALuint)sourceInfo->handle;
    alSourcefv(s, AL_POSITION, glm::value_ptr(position));
    alSourcefv(s, AL_VELOCITY, glm::value_ptr(velocity));
    alSourcefv(s, AL_DIRECTION, glm::value_ptr(direction));

    PrintALError("UpdateAudioSource");
}

void EnqueueAudioBuffer( AudioSource source, AudioBuffer buffer )
{
    const ALuint b = (ALuint)buffer;
    alSourceQueueBuffers((ALuint)source, 1, &b);
    PrintALError("EnqueueAudioBuffer");
}

void PlayAudioSource( AudioSource source )
{
    alSourcePlay((ALuint)source);
    PrintALError("PlayAudioSource");
}

void PauseAudioSource( AudioSource source )
{
    alSourcePause((ALuint)source);
    PrintALError("PauseAudioSource");
}

void PlayAllAudioSources( AudioSource source )
{
    for(int i = 0; i < AudioSourceCount; ++i)
        if(AudioSources[i].handle)
            PlayAudioSource(AudioSources[i].handle);
}

void PauseAllAudioSources( AudioSource source )
{
    for(int i = 0; i < AudioSourceCount; ++i)
        if(AudioSources[i].handle)
            PauseAudioSource(AudioSources[i].handle); // TODO: Maybe use a 'pause'-stack or so?
}
