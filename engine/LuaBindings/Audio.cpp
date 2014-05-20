#include "../Lua.h"
#include "../Audio.h"

const char* AUDIO_BUFFER_TYPE = "AudioBuffer";
const char* STOP_AUDIO_SOURCE_EVENT_NAME = "StopAudioSource";

int g_StopAudioSourceEvent = LUA_INVALID_EVENT;

int Lua_AudioBuffer_destructor( lua_State* l )
{
    const AudioBuffer* buffer =
        reinterpret_cast<AudioBuffer*>(lua_touserdata(l, 1));
    FreeAudioBuffer(*buffer);
    return 0;
}

int Lua_LoadAudioBuffer( lua_State* l ) // string fileName
{
    const char* fileName = luaL_checkstring(l, 1);

    const AudioBuffer buffer = LoadAudioBuffer(fileName);
    if(buffer)
    {
        if(CopyUserDataToLua(l, AUDIO_BUFFER_TYPE, sizeof(buffer), &buffer))
            return 1;
        else
            FreeAudioBuffer(buffer);
    }
    luaL_error(l, "Failed to create audio buffer.");
    return 0;
}

void OnStopLuaAudioSource( AudioSource source, void* context )
{
    lua_State* l = GetLuaState();
    lua_pushinteger(l, source);
    FireLuaEvent(l, g_StopAudioSourceEvent, 1, false);
}

int Lua_CreateAudioSource( lua_State* l )
{
    luaL_checktype(l, 1, LUA_TBOOLEAN);
    const bool triggerCallback = lua_toboolean(l, 1);

    const AudioSource handle = CreateAudioSource(
        triggerCallback ? OnStopLuaAudioSource : NULL,
        NULL
    );

    lua_pushinteger(l, handle);
    return 1;
}

int Lua_SetAudioSourceRelative( lua_State* l )
{
    const AudioSource source = (AudioSource)luaL_checkinteger(l, 1);

    luaL_checktype(l, 2, LUA_TBOOLEAN);
    const bool relative = lua_toboolean(l, 2);

    SetAudioSourceRelative(source, relative);
    return 0;
}

int Lua_SetAudioSourceLooping( lua_State* l )
{
    const AudioSource source = (AudioSource)luaL_checkinteger(l, 1);

    luaL_checktype(l, 2, LUA_TBOOLEAN);
    const bool loop = lua_toboolean(l, 2);

    SetAudioSourceRelative(source, loop);
    return 0;
}

int Lua_SetAudioSourcePitch( lua_State* l )
{
    const AudioSource source = (AudioSource)luaL_checkinteger(l, 1);
    const float pitch = luaL_checknumber(l, 2);
    SetAudioSourceRelative(source, pitch);
    return 0;
}

int Lua_SetAudioSourceGain( lua_State* l )
{
    const AudioSource source = (AudioSource)luaL_checkinteger(l, 1);
    const float gain = luaL_checknumber(l, 2);
    SetAudioSourceRelative(source, gain);
    return 0;
}

int Lua_EnqueueAudioBuffer( lua_State* l )
{
    const AudioSource source = (AudioSource)luaL_checkinteger(l, 1);

    const AudioBuffer buffer =
        *(AudioBuffer*)GetUserDataFromLua(l, 2, AUDIO_BUFFER_TYPE);

    EnqueueAudioBuffer(source, buffer);
    return 0;
}

int Lua_PlayAudioSource( lua_State* l )
{
    const AudioSource source = (AudioSource)luaL_checkinteger(l, 1);
    PlayAudioSource(source);
    return 0;
}

int Lua_PauseAudioSource( lua_State* l )
{
    const AudioSource source = (AudioSource)luaL_checkinteger(l, 1);
    PauseAudioSource(source);
    return 0;
}

int Lua_FreeAudioSource( lua_State* l )
{
    const AudioSource source = (AudioSource)luaL_checkinteger(l, 1);
    FreeAudioSource(source);
    return 0;
}

bool RegisterAudioInLua()
{
    if(!RegisterUserDataTypeInLua(AUDIO_BUFFER_TYPE, Lua_AudioBuffer_destructor))
        return false;

    g_StopAudioSourceEvent = RegisterLuaEvent(STOP_AUDIO_SOURCE_EVENT_NAME);
    if(g_StopAudioSourceEvent == LUA_INVALID_EVENT)
        return false;

    return
        RegisterFunctionInLua("LoadAudioBuffer", Lua_LoadAudioBuffer) &&
        RegisterFunctionInLua("CreateAudioSource", Lua_CreateAudioSource) &&
        RegisterFunctionInLua("SetAudioSourceRelative", Lua_SetAudioSourceRelative) &&
        RegisterFunctionInLua("SetAudioSourceLooping", Lua_SetAudioSourceLooping) &&
        RegisterFunctionInLua("SetAudioSourcePitch", Lua_SetAudioSourcePitch) &&
        RegisterFunctionInLua("SetAudioSourceGain", Lua_SetAudioSourceGain) &&
        RegisterFunctionInLua("EnqueueAudioBuffer", Lua_EnqueueAudioBuffer) &&
        RegisterFunctionInLua("PlayAudioSource", Lua_PlayAudioSource) &&
        RegisterFunctionInLua("PauseAudioSource", Lua_PauseAudioSource) &&
        RegisterFunctionInLua("FreeAudioSource", Lua_FreeAudioSource);
}
