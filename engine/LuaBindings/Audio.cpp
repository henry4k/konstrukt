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


// --- AudioBuffer ---

static int Lua_LoadAudioBuffer( lua_State* l ) // string fileName
{
    const char* fileName = luaL_checkstring(l, 1);

    AudioBuffer* buffer = LoadAudioBuffer(fileName);
    if(buffer)
    {
        PushPointerToLua(l, buffer);
        ReferenceAudioBuffer(buffer);
        return 1;
    }
    else
    {
        luaL_error(l, "Failed to create audio buffer.");
        return 0;
    }
}

static int Lua_DestroyAudioBuffer( lua_State* l )
{
    AudioBuffer* buffer = CheckAudioBufferFromLua(l, 1);
    ReleaseAudioBuffer(buffer);
    return 0;
}

AudioBuffer* GetAudioBufferFromLua( lua_State* l, int stackPosition )
{
    return (AudioBuffer*)GetPointerFromLua(l, stackPosition);
}

AudioBuffer* CheckAudioBufferFromLua( lua_State* l, int stackPosition )
{
    return (AudioBuffer*)CheckPointerFromLua(l, stackPosition);
}


// --- AudioSource ---

static int Lua_CreateAudioSource( lua_State* l )
{
    AudioSource* source = CreateAudioSource();
    if(source)
    {
        PushPointerToLua(l, source);
        ReferenceAudioSource(source);
        return 1;
    }
    else
    {
        luaL_error(l, "Failed to create audio source.");
        return 0;
    }
}

static int Lua_DestroyAudioSource( lua_State* l )
{
    AudioSource* source = CheckAudioSourceFromLua(l, 1);
    ReleaseAudioSource(source);
    return 0;
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
    return (AudioSource*)GetPointerFromLua(l, stackPosition);
}

AudioSource* CheckAudioSourceFromLua( lua_State* l, int stackPosition )
{
    return (AudioSource*)CheckPointerFromLua(l, stackPosition);
}


// --- Register in Lua ---

bool RegisterAudioInLua()
{
    return
        RegisterFunctionInLua("SetAudioListenerAttachmentTarget", Lua_SetAudioListenerAttachmentTarget) &&
        RegisterFunctionInLua("SetAudioListenerTransformation", Lua_SetAudioListenerTransformation) &&

        RegisterFunctionInLua("LoadAudioBuffer", Lua_LoadAudioBuffer) &&
        RegisterFunctionInLua("DestroyAudioBuffer", Lua_DestroyAudioBuffer) &&

        RegisterFunctionInLua("CreateAudioSource", Lua_CreateAudioSource) &&
        RegisterFunctionInLua("DestroyAudioSource", Lua_DestroyAudioSource) &&
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
