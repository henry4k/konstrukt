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


struct AudioBuffer
{
    ALuint handle;
};

struct AudioSource
{
    ALuint handle;

    AudioSourceStopFn stopFn;
    void* context;

    Solid* attachmentTarget;
    glm::mat4 transformation;
};

void FreeAudioSourceOnStop( AudioSource* source, void* context )
{
    FreeAudioSource(source);
}


static bool IsActiveAudioSource( AudioSource* source );
static void UpdateAudioListener();
static void UpdateAudioSource( AudioSource* source );
static const char* GetALErrorString();
static bool PrintALError( const char* origin );
static void PrintAudioDevices();

static AudioSource* AudioSources = NULL;
static int AudioSourceCount = 0;
static float MaxAudioSourceDistance = 0;
static float AudioSourceReferenceDistance = 0;
static Solid* ListenerAttachmentTarget = NULL;
static glm::mat4 ListenerTransformation;


// --- Global ---

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

    return true;
}

void DestroyAudio()
{
    SetAudioListenerAttachmentTarget(NULL);

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


// --- AudioBuffer ---

AudioBuffer* LoadAudioBuffer( const char* fileName )
{
    const ALuint handle = alureCreateBufferFromFile(fileName);
    if(handle == AL_NONE || PrintALError("LoadAudioBuffer"))
    {
        Error("Can't load %s: %s", fileName, alureGetErrorString());
        return NULL;
    }
    else
    {
        Log("Loaded %s", fileName);
        AudioBuffer* buffer = new AudioBuffer;
        memset(buffer, 0, sizeof(AudioBuffer));
        buffer->handle = handle;
        return buffer;
    }
}

void FreeAudioBuffer( AudioBuffer* buffer )
{
    alDeleteBuffers(1, &buffer->handle);
    PrintALError("FreeAudioBuffer");
    delete buffer;
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

AudioSource* CreateAudioSource( AudioSourceStopFn stopFn, void* context )
{
    AudioSource* source = FindFreeAudioSource();
    if(source)
    {
        ALuint handle = AL_NONE;
        alGenSources(1, &handle);

        source->handle = handle;
        source->stopFn = stopFn;
        source->context = context;

        alSourcef(handle, AL_MAX_DISTANCE, MaxAudioSourceDistance);
        alSourcef(handle, AL_REFERENCE_DISTANCE, AudioSourceReferenceDistance);

        return source;
    }
    else
    {
        Error("Reached audio source limit! (%d sources)", AudioSourceCount);
        return NULL;
    }
}

void FreeAudioSource( AudioSource* source )
{
    alDeleteSources(1, &source->handle);
    memset(source, 0, sizeof(AudioSource));
    source->handle = AL_NONE;
    PrintALError("FreeAudioSourceAtIndex");
}

void SetAudioSourceRelative( AudioSource* source, bool relative )
{
    alSourcei(source->handle, AL_SOURCE_RELATIVE, (relative ? AL_TRUE : AL_FALSE));
    PrintALError("SetAudioSourceRelative");
}

void SetAudioSourceLooping( AudioSource* source, bool loop )
{
    alSourcei(source->handle, AL_LOOPING, (loop ? AL_TRUE : AL_FALSE));
    PrintALError("SetAudioSourceLooping");
}

void SetAudioSourcePitch( AudioSource* source, float pitch )
{
    alSourcef(source->handle, AL_PITCH, pitch);
    PrintALError("SetAudioSourcePitch");
}

void SetAudioSourceGain( AudioSource* source, float gain )
{
    alSourcef(source->handle, AL_GAIN, gain);
    PrintALError("SetAudioSourceGain");
}

void SetAudioSourceAttachmentTarget( AudioSource* source, Solid* target )
{
    if(source->attachmentTarget)
        ReleaseSolid(source->attachmentTarget);
    source->attachmentTarget = target;
    if(source->attachmentTarget)
        ReferenceSolid(source->attachmentTarget);
}

void SetAudioSourceTransformation( AudioSource* source, glm::mat4 transformation )
{
    source->transformation = transformation;
}

static void UpdateAudioSource( AudioSource* source )
{
    if(source->stopFn)
    {
        int state = 0;
        alGetSourcei(source->handle, AL_SOURCE_STATE, &state);
        if(!PrintALError("UpdateAudio") && (state == AL_STOPPED || state == AL_INITIAL)) // TODO: Check enum values
        {
            source->stopFn(source, source->context);
            return;
        }
    }

    using namespace glm;

    mat4 targetTransformation;
    if(source->attachmentTarget)
        GetSolidTransformation(source->attachmentTarget, &targetTransformation);
    const mat4 finalTransformation = targetTransformation *
                                     source->transformation;

    // TODO: Retrieve velocity and orientation.
    const vec3 position(finalTransformation * vec4());
    const vec3 velocity;
    const vec3 direction(0,0,1);
    const vec3 up(0,1,0);

    alSourcefv(source->handle, AL_POSITION, glm::value_ptr(position));
    alSourcefv(source->handle, AL_VELOCITY, glm::value_ptr(velocity));
    alSourcefv(source->handle, AL_DIRECTION, glm::value_ptr(direction));

    PrintALError("UpdateAudioSource");
}

void EnqueueAudioBuffer( AudioSource* source, AudioBuffer* buffer )
{
    alSourceQueueBuffers(source->handle, 1, &buffer->handle);
    PrintALError("EnqueueAudioBuffer");
}

void PlayAudioSource( AudioSource* source )
{
    alSourcePlay(source->handle);
    PrintALError("PlayAudioSource");
}

void PauseAudioSource( AudioSource* source )
{
    alSourcePause(source->handle);
    PrintALError("PauseAudioSource");
}

void PlayAllAudioSources()
{
    for(int i = 0; i < AudioSourceCount; ++i)
        if(!IsActiveAudioSource(&AudioSources[i]))
            PlayAudioSource(&AudioSources[i]);
}

void PauseAllAudioSources()
{
    for(int i = 0; i < AudioSourceCount; ++i)
        if(!IsActiveAudioSource(&AudioSources[i]))
            PauseAudioSource(&AudioSources[i]); // TODO: Maybe use a 'pause'-stack or so?
}


// --- Utils ---

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
