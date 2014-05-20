#include <string.h> // memset

#include "../Lua.h"
#include "../Mesh.h"
#include "../RenderManager.h"
#include "Mesh.h"
#include "RenderManager.h"


const char* GRAPHICS_OBJECT_TYPE = "GraphicsObject";

int Lua_GraphicsObject_destructor( lua_State* l )
{
    GraphicsObject* object =
        reinterpret_cast<GraphicsObject*>(lua_touserdata(l, 1));
    FreeGraphicsObject(object);
    return 0;
}

int Lua_CreateGraphicsObject( lua_State* l )
{
    GraphicsObject* object = CreateGraphicsObject();
    if(object)
    {
        if(CopyUserDataToLua(l, GRAPHICS_OBJECT_TYPE, sizeof(object), &object))
            return 1;
        else
            FreeGraphicsObject(object);
    }
    else
    {
        luaL_error(l, "Can't create more graphics objects.");
    }
    return 0;
}

int Lua_SetGraphicsObjectMesh( lua_State* l )
{
    GraphicsObject* object = CheckGraphicsObjectFromLua(l, 1);
    Mesh* mesh = CheckMeshFromLua(l, 2);
    object->mesh = mesh;
    return 0;
}

int Lua_SetGraphicsObjectTransformation( lua_State* l )
{
    GraphicsObject* object = CheckGraphicsObjectFromLua(l, 1);
    glm::mat4* transformation = (glm::mat4*)lua_touserdata(l, 2);
    object->transformation = *transformation;
    return 0;
}

int Lua_RemoveGraphicsObject( lua_State* l )
{
    GraphicsObject* object = CheckGraphicsObjectFromLua(l, 1);
    object->active = false;
    return 0;
}

bool RegisterRenderManagerInLua()
{
    if(!RegisterUserDataTypeInLua(GRAPHICS_OBJECT_TYPE, Lua_GraphicsObject_destructor))
        return false;

    return
        RegisterFunctionInLua("CreateGraphicsObject", Lua_CreateGraphicsObject) &&
        RegisterFunctionInLua("SetGraphicsObjectMesh", Lua_SetGraphicsObjectMesh) &&
        RegisterFunctionInLua("SetGraphicsObjectTransformation", Lua_SetGraphicsObjectTransformation) &&
        RegisterFunctionInLua("RemoveGraphicsObject", Lua_RemoveGraphicsObject);
}

GraphicsObject* GetGraphicsObjectFromLua( lua_State* l, int stackPosition )
{
    return *(GraphicsObject**)GetUserDataFromLua(l, stackPosition, GRAPHICS_OBJECT_TYPE);
}

GraphicsObject* CheckGraphicsObjectFromLua( lua_State* l, int stackPosition )
{
    return *(GraphicsObject**)CheckUserDataFromLua(l, stackPosition, GRAPHICS_OBJECT_TYPE);
}
