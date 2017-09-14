#include "../Common.h"
#include "../Lua.h"
#include "PhysicsWorld.h"
#include "AttachmentTarget.h"


static const char* ATTACHMENT_TARGET_TYPE = "AttachmentTarget";

static int Lua_CreateAttachmentTarget( lua_State* l )
{
    AttachmentTarget* target =
        (AttachmentTarget*)PushUserDataToLua(l,
                                             ATTACHMENT_TARGET_TYPE,
                                             sizeof(AttachmentTarget));
    InitAttachmentTarget(target);
    return 1;
}

static int Lua_DestroyAttachmentTarget( lua_State* l )
{
    AttachmentTarget* target = CheckAttachmentTargetFromLua(l, 1);
    DestroyAttachmentTarget(target);
    return 0;
}

static int Lua_SetAttachmentTargetFilter( lua_State* l )
{
    AttachmentTarget* target = CheckAttachmentTargetFromLua(l, 1);
    int filter = 0;

    REPEAT(lua_gettop(l)-1, i)
    {
        static const char* filterTypes[] =
        {
            "position",
            "rotation",
            "linear velocity",
            NULL
        };
        const int filterType = luaL_checkoption(l, 2+i, NULL, filterTypes);
        filter |= (1 << filterType);
    }

    SetAttachmentTargetFilter(target, filter);

    return 0;
}

static int Lua_SetSolidAttachmentTarget( lua_State* l )
{
    AttachmentTarget* target = CheckAttachmentTargetFromLua(l, 1);
    PhysicsWorld* world = CheckPhysicsWorldFromLua(l, 2);
    SolidId solid = CheckSolidFromLua(l, 3);
    SetSolidAsAttachmentTarget(target, world, solid);
    return 0;
}

static int Lua_UnsetAttachmentTarget( lua_State* l )
{
    AttachmentTarget* target = CheckAttachmentTargetFromLua(l, 1);
    UnsetAttachmentTarget(target);
    return 0;
}

AttachmentTarget* GetAttachmentTargetFromLua( lua_State* l, int stackPosition )
{
    return (AttachmentTarget*)GetUserDataFromLua(l, stackPosition, ATTACHMENT_TARGET_TYPE);
}

AttachmentTarget* CheckAttachmentTargetFromLua( lua_State* l, int stackPosition )
{
    return (AttachmentTarget*)CheckUserDataFromLua(l, stackPosition, ATTACHMENT_TARGET_TYPE);
}


// --- Register in Lua ---

void RegisterAttachmentTargetInLua()
{
    RegisterUserDataTypeInLua(ATTACHMENT_TARGET_TYPE, NULL);
    RegisterFunctionInLua("CreateAttachmentTarget", Lua_CreateAttachmentTarget);
    RegisterFunctionInLua("DestroyAttachmentTarget", Lua_DestroyAttachmentTarget);
    RegisterFunctionInLua("SetAttachmentTargetFilter", Lua_SetAttachmentTargetFilter);
    RegisterFunctionInLua("SetSolidAsAttachmentTarget", Lua_SetSolidAttachmentTarget);
    RegisterFunctionInLua("UnsetAttachmentTarget", Lua_UnsetAttachmentTarget);
}
