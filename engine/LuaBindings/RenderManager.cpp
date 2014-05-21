#include <string.h> // memset

#include "../Lua.h"
#include "../Mesh.h"
#include "../RenderManager.h"
#include "Mesh.h"
#include "RenderManager.h"


const char* MODEL_TYPE = "Model";

int Lua_Model_destructor( lua_State* l )
{
    Model* model =
        reinterpret_cast<Model*>(lua_touserdata(l, 1));
    FreeModel(model);
    return 0;
}

int Lua_CreateModel( lua_State* l )
{
    Model* model = CreateModel();
    if(model)
    {
        if(CopyUserDataToLua(l, MODEL_TYPE, sizeof(model), &model))
            return 1;
        else
            FreeModel(model);
    }
    else
    {
        luaL_error(l, "Can't create more models.");
    }
    return 0;
}

int Lua_SetModelTransformation( lua_State* l )
{
    Model* model = CheckModelFromLua(l, 1);
    glm::mat4* transformation = (glm::mat4*)lua_touserdata(l, 2);
    SetModelTransformation(model, *transformation);
    return 0;
}

int Lua_SetModelMesh( lua_State* l )
{
    Model* model = CheckModelFromLua(l, 1);
    Mesh* mesh = CheckMeshFromLua(l, 2);
    SetModelMesh(model, mesh);
    return 0;
}

int Lua_FreeModel( lua_State* l )
{
    Model* model = CheckModelFromLua(l, 1);
    FreeModel(model);
    return 0;
}

bool RegisterRenderManagerInLua()
{
    if(!RegisterUserDataTypeInLua(MODEL_TYPE, Lua_Model_destructor))
        return false;

    return
        RegisterFunctionInLua("CreateModel", Lua_CreateModel) &&
        RegisterFunctionInLua("FreeModel", Lua_FreeModel) &&
        RegisterFunctionInLua("SetModelTransformation", Lua_SetModelTransformation) &&
        RegisterFunctionInLua("SetModelMesh", Lua_SetModelMesh);
}

Model* GetModelFromLua( lua_State* l, int stackPosition )
{
    return *(Model**)GetUserDataFromLua(l, stackPosition, MODEL_TYPE);
}

Model* CheckModelFromLua( lua_State* l, int stackPosition )
{
    return *(Model**)CheckUserDataFromLua(l, stackPosition, MODEL_TYPE);
}
