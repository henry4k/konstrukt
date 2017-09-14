#ifndef __KONSTRUKT_LUA_BINDINGS_ATTACHMENT_TARGET__
#define __KONSTRUKT_LUA_BINDINGS_ATTACHMENT_TARGET__

#include "../AttachmentTarget.h"

struct lua_State;

AttachmentTarget* GetAttachmentTargetFromLua( lua_State* l, int stackPosition );
AttachmentTarget* CheckAttachmentTargetFromLua( lua_State* l, int stackPosition );

void RegisterAttachmentTargetInLua();

#endif
