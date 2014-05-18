#ifndef __APOAPSIS_RENDER_MANAGER__
#define __APOAPSIS_RENDER_MANAGER__

#include "Math.h"
#include "Shader.h"
#include "Texture.h"
#include "Lua.h"

struct Mesh;
struct PhysicsObject;


enum GraphicsFlags
{
     // nothing here yet
};

struct GraphicsObject
{
    bool active;
    PhysicsObject* attachTarget;
    glm::mat4 transformation;
    int renderFlags;
    Texture diffuseTexture;
    Mesh* mesh;
};

bool InitRenderManager();
void DestroyRenderManager();
void DrawGraphicsObjects( const glm::mat4* mvpMatrix );

GraphicsObject* CreateGraphicsObject();
void FreeGraphicsObject( GraphicsObject* object );

GraphicsObject* GetGraphicsObjectFromLua( lua_State* l, int stackPosition );
GraphicsObject* CheckGraphicsObjectFromLua( lua_State* l, int stackPosition );

#endif
