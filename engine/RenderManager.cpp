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

    DefaultProgram = LoadProgram("core/Shaders/Test.vert", "core/Shaders/Test.frag");
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
