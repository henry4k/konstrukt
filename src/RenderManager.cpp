#include <string.h> // memset

#include "Common.h"
#include "PhysicsManager.h" // PhysicsObject
#include "Mesh.h"
#include "RenderManager.h"


static const int MAX_GRAPHICS_OBJECTS = 8;
static GraphicsObject GraphicsObjects[MAX_GRAPHICS_OBJECTS];

static Program DefaultProgram;

bool InitRenderManager()
{
    memset(GraphicsObjects, 0, sizeof(GraphicsObjects));

    DefaultProgram = LoadProgram("Shaders/Test.vert", "Shaders/Test.frag");
    BindVertexAttributes(DefaultProgram);

    return true;
}

void DestroyRenderManager()
{
    FreeProgram(DefaultProgram);

    for(int i = 0; i < MAX_GRAPHICS_OBJECTS; i++)
        if(GraphicsObjects[i].active)
            Error("Graphics object #%d (%p) was still active when the manager was destroyed.",
                i, &GraphicsObjects[i]);
}

void DrawGraphicsObjects( const glm::mat4* mvpMatrix )
{
    // Naive draw method:

    BindProgram(DefaultProgram);
    SetUniformMatrix4(DefaultProgram, "MVP", mvpMatrix);

    for(int i = 0; i < MAX_GRAPHICS_OBJECTS; i++)
    {
        const GraphicsObject* object = &GraphicsObjects[i];
        if(object->active)
        {
            BindTexture(GL_TEXTURE_2D, object->diffuseTexture, 0);
            DrawMesh(object->mesh);
        }
    }
}

static GraphicsObject* FindInactiveGraphicsObject()
{
    for(int i = 0; i < MAX_GRAPHICS_OBJECTS; i++)
        if(!GraphicsObjects[i].active)
            return &GraphicsObjects[i];
    return NULL;
}

GraphicsObject* CreateGraphicsObject()
{
    GraphicsObject* object = FindInactiveGraphicsObject();
    if(object)
    {
        memset(object, 0, sizeof(GraphicsObject));
        object->active = true;
        return object;
    }
    else
    {
        Error("Can't create more graphics objects.");
        return NULL;
    }
}

void FreeGraphicsObject( GraphicsObject* object )
{
    object->active = false;
}


// --- lua bindings ---

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

AutoRegisterInLua()
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
