#ifndef __KONSTRUKT_LUA_BINDINGS_AUDIO__
#define __KONSTRUKT_LUA_BINDINGS_AUDIO__

struct lua_State;
struct AudioBuffer;
struct AudioSource;

AudioBuffer* GetAudioBufferFromLua( lua_State* l, int stackPosition );
AudioBuffer* CheckAudioBufferFromLua( lua_State* l, int stackPosition );

AudioSource* GetAudioSourceFromLua( lua_State* l, int stackPosition );
AudioSource* CheckAudioSourceFromLua( lua_State* l, int stackPosition );

bool RegisterAudioInLua();

#endif
