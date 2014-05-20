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
#include "Audio.h"

struct AudioSourceInfo
{
    AudioSource handle;
    AudioSourceStopFn stopFn;
    void* context;
};

void FreeAudioSourceOnStop( AudioSource source, void* context )
{
    FreeAudioSource(source);
}

void FreeAudioSourceAtIndex( int index );

AudioSourceInfo* g_AudioSources;
int g_AudioSourceCount;

float g_MaxAudioSourceDistance;
float g_AudioSourceReferenceDistance;

const char* GetALErrorString()
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

bool PrintALError( const char* origin )
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

void PrintAudioDevices()
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

    g_AudioSourceCount = GetConfigInt("audio.max-sources", 32);
    g_AudioSources = new AudioSourceInfo[g_AudioSourceCount];
    memset(g_AudioSources, 0, sizeof(AudioSourceInfo)*g_AudioSourceCount);

    g_MaxAudioSourceDistance = GetConfigFloat("audio.max-distance", 100);
    g_AudioSourceReferenceDistance = GetConfigFloat("audio.reference-distance", 100);

    SetAudioGain(GetConfigFloat("audio.gain", 1.0f));

    // TODO: Set distance model, doppler factor and so on!
    alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED); // ?
    //alDopplerFactor();
    //alDopplerVelocity();

    return true;
}

void DestroyAudio()
{
    for(int i = 0; i < g_AudioSourceCount; ++i)
        if(g_AudioSources[i].handle)
            alDeleteSources(1, &g_AudioSources[i].handle);
    delete[] g_AudioSources;

    alureShutdownDevice();
}

void UpdateAudio( float timeFrame )
{
    for(int i = 0; i < g_AudioSourceCount; ++i)
    {
        const AudioSourceInfo* sourceInfo = &g_AudioSources[i];

        if(sourceInfo->handle && sourceInfo->stopFn)
        {
            int state = 0;
            alGetSourcei(sourceInfo->handle, AL_SOURCE_STATE, &state);

            if(!PrintALError("InitAudio") && (state == AL_STOPPED || state == AL_INITIAL)) // TODO: Check enum values
            {
                sourceInfo->stopFn( sourceInfo->handle, sourceInfo->context );
            }
        }
    }
}

void UpdateAudioListener( glm::vec3 position, glm::vec3 velocity, glm::vec3 direction, glm::vec3 up )
{
    float orientation[6];
    memcpy(orientation,   glm::value_ptr(direction), sizeof(float)*3);
    memcpy(orientation+3, glm::value_ptr(up),        sizeof(float)*3);

    alListenerfv(AL_POSITION, glm::value_ptr(position));
    alListenerfv(AL_VELOCITY, glm::value_ptr(velocity));
    alListenerfv(AL_ORIENTATION, orientation);
}

void SetAudioGain( float gain )
{
    alListenerf(AL_GAIN, gain);
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
    for(int i = 0; i < g_AudioSourceCount; ++i)
    {
        AudioSourceInfo* sourceInfo = &g_AudioSources[i];
        if(sourceInfo->handle == AL_NONE)
        {
            ALuint source = AL_NONE;
            alGenSources(1, &source);

            sourceInfo->handle = source;
            sourceInfo->stopFn = stopFn;
            sourceInfo->context = context;

            alSourcef(source, AL_MAX_DISTANCE, g_MaxAudioSourceDistance);
            alSourcef(source, AL_REFERENCE_DISTANCE, g_AudioSourceReferenceDistance);

            return (AudioSource)source;
        }
    }
    Error("Reached audio source limit! (%d sources)", g_AudioSourceCount);
    return 0;
}

void FreeAudioSourceAtIndex( int index )
{
    AudioSourceInfo* sourceInfo = &g_AudioSources[index];
    alDeleteSources(1, &sourceInfo->handle);
    memset(sourceInfo, 0, sizeof(AudioSourceInfo));
    PrintALError("FreeAudioSourceAtIndex");
}

void FreeAudioSource( AudioSource source )
{
    for(int i = 0; i < g_AudioSourceCount; ++i)
    {
        if(g_AudioSources[i].handle == source)
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
    for(int i = 0; i < g_AudioSourceCount; ++i)
        if(g_AudioSources[i].handle)
            PlayAudioSource(g_AudioSources[i].handle);
}

void PauseAllAudioSources( AudioSource source )
{
    for(int i = 0; i < g_AudioSourceCount; ++i)
        if(g_AudioSources[i].handle)
            PauseAudioSource(g_AudioSources[i].handle); // TODO: Maybe use a 'pause'-stack or so?
}

void UpdateAudioSource( AudioSource source, glm::vec3 position, glm::vec3 velocity, glm::vec3 direction )
{
    const ALuint s = (ALuint)source;
    alSourcefv(s, AL_POSITION, glm::value_ptr(position));
    alSourcefv(s, AL_VELOCITY, glm::value_ptr(velocity));
    alSourcefv(s, AL_DIRECTION, glm::value_ptr(direction));
    PrintALError("UpdateAudioSource");
}