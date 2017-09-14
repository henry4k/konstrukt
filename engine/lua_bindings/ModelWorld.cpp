#include <string.h> // memset

#include "../Lua.h"
#include "../Mesh.h"
#include "../Shader.h"
#include "../ModelWorld.h"
#include "Math.h"
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include "AttachmentTarget.h"
#include "ModelWorld.h"


// ---- ModelWorld ----

static int Lua_CreateModelWorld( lua_State* l )
{
    ModelWorld* world = CreateModelWorld();
    if(world)
    {
        PushPointerToLua(l, world);
        ReferenceModelWorld(world);
        return 1;
    }
    else
    {
        return luaL_error(l, "Can't create more model worlds.");
    }
}

static int Lua_DestroyModelWorld( lua_State* l )
{
    ModelWorld* world = CheckModelWorldFromLua(l, 1);
    ReleaseModelWorld(world);
    return 0;
}

ModelWorld* GetModelWorldFromLua( lua_State* l, int stackPosition )
{
    return (ModelWorld*)GetPointerFromLua(l, stackPosition);
}

ModelWorld* CheckModelWorldFromLua( lua_State* l, int stackPosition )
{
    return (ModelWorld*)CheckPointerFromLua(l, stackPosition);
}


// ---- Model ----

static int Lua_CreateModel( lua_State* l )
{
    ModelWorld* world = CheckModelWorldFromLua(l, 1);

    Model* model = CreateModel(world);
    if(model)
    {
        PushPointerToLua(l, model);
        ReferenceModel(model);
        return 1;
    }
    else
    {
        return luaL_error(l, "Can't create more models.");
    }
}

static int Lua_DestroyModel( lua_State* l )
{
    Model* model = CheckModelFromLua(l, 1);
    ReleaseModel(model);
    return 0;
}

static int Lua_SetModelAttachmentTarget( lua_State* l )
{
    Model* model = CheckModelFromLua(l, 1);
    const AttachmentTarget* target = GetAttachmentTargetFromLua(l, 2);
    SetModelAttachmentTarget(model, target);
    return 0;
}

static int Lua_SetModelTransformation( lua_State* l )
{
    Model* model = CheckModelFromLua(l, 1);
    const Mat4* transformation = CheckMatrix4FromLua(l, 2);
    SetModelTransformation(model, *transformation);
    return 0;
}

static int Lua_SetModelOverlayLevel( lua_State* l )
{
    Model* model = CheckModelFromLua(l, 1);
    const int level = luaL_checkinteger(l, 2);
    SetModelOverlayLevel(model, level);
    return 0;
}

static int Lua_SetModelMesh( lua_State* l )
{
    Model* model = CheckModelFromLua(l, 1);
    Mesh* mesh = CheckMeshFromLua(l, 2);
    SetModelMesh(model, mesh);
    return 0;
}

static int Lua_SetModelProgramFamilyList( lua_State* l )
{
    Model* model = CheckModelFromLua(l, 1);
    const char* familyList = luaL_checkstring(l, 2);
    SetModelProgramFamilyList(model, familyList);
    return 0;
}

static int Lua_GetModelShaderVariableSet( lua_State* l )
{
    Model* model = CheckModelFromLua(l, 1);
    PushShaderVariableSetToLua(l, GetModelShaderVariableSet(model));
    return 1;
}

Model* GetModelFromLua( lua_State* l, int stackPosition )
{
    return (Model*)GetPointerFromLua(l, stackPosition);
}

Model* CheckModelFromLua( lua_State* l, int stackPosition )
{
    return (Model*)CheckPointerFromLua(l, stackPosition);
}

void RegisterModelWorldInLua()
{
    RegisterFunctionInLua("CreateModelWorld", Lua_CreateModelWorld);
    RegisterFunctionInLua("DestroyModelWorld", Lua_DestroyModelWorld);

    RegisterFunctionInLua("CreateModel", Lua_CreateModel);
    RegisterFunctionInLua("DestroyModel", Lua_DestroyModel);
    RegisterFunctionInLua("SetModelAttachmentTarget", Lua_SetModelAttachmentTarget);
    RegisterFunctionInLua("SetModelTransformation", Lua_SetModelTransformation);
    RegisterFunctionInLua("SetModelOverlayLevel", Lua_SetModelOverlayLevel);
    RegisterFunctionInLua("SetModelMesh", Lua_SetModelMesh);
    RegisterFunctionInLua("SetModelProgramFamilyList", Lua_SetModelProgramFamilyList);
    RegisterFunctionInLua("GetModelShaderVariableSet", Lua_GetModelShaderVariableSet);
}
