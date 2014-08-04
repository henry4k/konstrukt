#include "../Lua.h"
#include "../Audio.h"
#include "Math.h"
#include "PhysicsManager.h"
#include "Audio.h"


// --- AudioListener ---

static int Lua_SetAudioListenerAttachmentTarget( lua_State* l )
{
    Solid* target = CheckSolidFromLua(l, 1);
    SetAudioListenerAttachmentTarget(target);
    return 0;
}

static int Lua_SetAudioListenerTransformation( lua_State* l )
{
    const glm::mat4* transformation = CheckMatrix4FromLua(l, 1);
    SetAudioListenerTransformation(*transformation);
    return 0;
}

static int Lua_PlayAllAudioSources( lua_State* l )
{
    PlayAllAudioSources();
    return 0;
}

static int Lua_PauseAllAudioSources( lua_State* l )
{
    PauseAllAudioSources();
    return 0;
}


// --- AudioBuffer ---

static const char* AUDIO_BUFFER_TYPE = "AudioBuffer";

static int Lua_AudioBuffer_destructor( lua_State* l )
{
    AudioBuffer* buffer = CheckAudioBufferFromLua(l, 1);
    ReleaseAudioBuffer(buffer);
    return 0;
}

static int Lua_LoadAudioBuffer( lua_State* l ) // string fileName
{
    const char* fileName = luaL_checkstring(l, 1);

    AudioBuffer* buffer = LoadAudioBuffer(fileName);
    if(buffer && CopyUserDataToLua(l, AUDIO_BUFFER_TYPE, sizeof(buffer), &buffer))
    {
        ReferenceAudioBuffer(buffer);
        return 1;
    }
    else
    {
        ReleaseAudioBuffer(buffer);
        luaL_error(l, "Failed to create audio buffer.");
        return 0;
    }
}

AudioBuffer* GetAudioBufferFromLua( lua_State* l, int stackPosition )
{
    return *(AudioBuffer**)GetUserDataFromLua(l, stackPosition, AUDIO_BUFFER_TYPE);
}

AudioBuffer* CheckAudioBufferFromLua( lua_State* l, int stackPosition )
{
    return *(AudioBuffer**)CheckUserDataFromLua(l, stackPosition, AUDIO_BUFFER_TYPE);
}


// --- AudioSource ---

static const char* STOP_AUDIO_SOURCE_EVENT_NAME = "StopAudioSource";

static int StopAudioSourceEvent = INVALID_LUA_EVENT;

static const char* AUDIO_SOURCE_TYPE = "AudioSource";

static int Lua_AudioSource_destructor( lua_State* l )
{
    AudioSource* source = CheckAudioSourceFromLua(l, 1);
    ReleaseAudioSource(source);
    return 0;
}

static bool PushAudioSourceToLua( lua_State* l, AudioSource* source )
{
    return CopyUserDataToLua(l, AUDIO_SOURCE_TYPE, sizeof(source), &source);
}

static void OnStopLuaAudioSource( AudioSource* source, void* context )
{
    lua_State* l = GetLuaState();
    PushAudioSourceToLua(l, source);
    FireLuaEvent(l, StopAudioSourceEvent, 1, false);
}

static int Lua_CreateAudioSource( lua_State* l )
{
    luaL_checktype(l, 1, LUA_TBOOLEAN);
    const bool triggerCallback = lua_toboolean(l, 1);
    AudioSourceStopFn callback = triggerCallback ? OnStopLuaAudioSource :
                                                   NULL;
    AudioSource* source = CreateAudioSource(callback, NULL);
    if(source && PushAudioSourceToLua(l, source))
    {
        ReferenceAudioSource(source);
        return 1;
    }
    else
    {
        ReleaseAudioSource(source);
        luaL_error(l, "Failed to create audio source.");
        return 0;
    }
}

static int Lua_SetAudioSourceRelative( lua_State* l )
{
    AudioSource* source = CheckAudioSourceFromLua(l, 1);

    luaL_checktype(l, 2, LUA_TBOOLEAN);
    const bool relative = lua_toboolean(l, 2);

    SetAudioSourceRelative(source, relative);
    return 0;
}

static int Lua_SetAudioSourceLooping( lua_State* l )
{
    AudioSource* source = CheckAudioSourceFromLua(l, 1);

    luaL_checktype(l, 2, LUA_TBOOLEAN);
    const bool loop = lua_toboolean(l, 2);

    SetAudioSourceRelative(source, loop);
    return 0;
}

static int Lua_SetAudioSourcePitch( lua_State* l )
{
    AudioSource* source = CheckAudioSourceFromLua(l, 1);
    const float pitch = luaL_checknumber(l, 2);
    SetAudioSourceRelative(source, pitch);
    return 0;
}

static int Lua_SetAudioSourceGain( lua_State* l )
{
    AudioSource* source = CheckAudioSourceFromLua(l, 1);
    const float gain = luaL_checknumber(l, 2);
    SetAudioSourceRelative(source, gain);
    return 0;
}

static int Lua_SetAudioSourceAttachmentTarget( lua_State* l )
{
    AudioSource* source = CheckAudioSourceFromLua(l, 1);
    Solid* target = CheckSolidFromLua(l, 2);
    SetAudioSourceAttachmentTarget(source, target);
    return 0;
}

static int Lua_SetAudioSourceTransformation( lua_State* l )
{
    AudioSource* source = CheckAudioSourceFromLua(l, 1);
    const glm::mat4* transformation = CheckMatrix4FromLua(l, 2);
    SetAudioSourceTransformation(source, *transformation);
    return 0;
}

static int Lua_EnqueueAudioBuffer( lua_State* l )
{
    AudioSource* source = CheckAudioSourceFromLua(l, 1);
    AudioBuffer* buffer = CheckAudioBufferFromLua(l, 2);
    EnqueueAudioBuffer(source, buffer);
    return 0;
}

static int Lua_PlayAudioSource( lua_State* l )
{
    AudioSource* source = CheckAudioSourceFromLua(l, 1);
    PlayAudioSource(source);
    return 0;
}

static int Lua_PauseAudioSource( lua_State* l )
{
    AudioSource* source = CheckAudioSourceFromLua(l, 1);
    PauseAudioSource(source);
    return 0;
}

AudioSource* GetAudioSourceFromLua( lua_State* l, int stackPosition )
{
    return *(AudioSource**)GetUserDataFromLua(l, stackPosition, AUDIO_SOURCE_TYPE);
}

AudioSource* CheckAudioSourceFromLua( lua_State* l, int stackPosition )
{
    return *(AudioSource**)CheckUserDataFromLua(l, stackPosition, AUDIO_SOURCE_TYPE);
}


// --- Register in Lua ---

bool RegisterAudioInLua()
{
    StopAudioSourceEvent = RegisterLuaEvent(STOP_AUDIO_SOURCE_EVENT_NAME);
    if(StopAudioSourceEvent == INVALID_LUA_EVENT)
        return false;

    return
        RegisterFunctionInLua("SetAudioListenerAttachmentTarget", Lua_SetAudioListenerAttachmentTarget) &&
        RegisterFunctionInLua("SetAudioListenerTransformation", Lua_SetAudioListenerTransformation) &&
        RegisterFunctionInLua("PlayAllAudioSources", Lua_PlayAllAudioSources) &&
        RegisterFunctionInLua("PauseAllAudioSources", Lua_PauseAllAudioSources) &&

        RegisterUserDataTypeInLua(AUDIO_BUFFER_TYPE, Lua_AudioBuffer_destructor) &&
        RegisterFunctionInLua("LoadAudioBuffer", Lua_LoadAudioBuffer) &&

        RegisterUserDataTypeInLua(AUDIO_SOURCE_TYPE, Lua_AudioSource_destructor) &&
        RegisterFunctionInLua("CreateAudioSource", Lua_CreateAudioSource) &&
        RegisterFunctionInLua("SetAudioSourceRelative", Lua_SetAudioSourceRelative) &&
        RegisterFunctionInLua("SetAudioSourceLooping", Lua_SetAudioSourceLooping) &&
        RegisterFunctionInLua("SetAudioSourcePitch", Lua_SetAudioSourcePitch) &&
        RegisterFunctionInLua("SetAudioSourceGain", Lua_SetAudioSourceGain) &&
        RegisterFunctionInLua("SetAudioSourceAttachmentTarget", Lua_SetAudioSourceAttachmentTarget) &&
        RegisterFunctionInLua("SetAudioSourceTransformation", Lua_SetAudioSourceTransformation) &&
        RegisterFunctionInLua("EnqueueAudioBuffer", Lua_EnqueueAudioBuffer) &&
        RegisterFunctionInLua("PlayAudioSource", Lua_PlayAudioSource) &&
        RegisterFunctionInLua("PauseAudioSource", Lua_PauseAudioSource);
}
