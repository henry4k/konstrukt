#include "Math.h"
#include <glm/gtc/matrix_transform.hpp>
#include "OpenGL.h"
#include "Vertex.h"
#include "Window.h"
#include "Reference.h"
#include "PhysicsManager.h"
#include "ModelWorld.h"
#include "Camera.h"
#include "RenderManager.h"


Camera* MainCamera = NULL;
ModelWorld* MainModelWorld = NULL;

static void UpdateProjectionTransformation();
static void OnFramebufferResize( int width, int height );

bool InitRenderManager()
{
    EnableVertexArrays();
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    //glEnable(GL_CULL_FACE);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.5, 0.5, 0.5, 1);

    SetFrambufferFn(OnFramebufferResize);
    return true;
}

void DestroyRenderManager()
{
}

static void OnFramebufferResize( int width, int height )
{
    glViewport(0, 0, width, height);
    UpdateProjectionTransformation();
}

static void UpdateProjectionTransformation()
{
    using namespace glm;
    const ivec2 framebufferSize = GetFramebufferSize();
    UpdateCameraProjection(MainCamera, framebufferSize);
}

void RenderScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    DrawModelWorld(MainModelWorld, NULL, MainCamera);
    SwapBuffers();

    /*
    // Render shadow map
    BeginRenderShadowTexture();
    glClear(GL_DEPTH_BUFFER_BIT);
    DrawGraphicsObjects();
    DrawPlayer();
    EndRenderShadowTexture();

    // Render world
    BeginRender();
    glClear(GL_DEPTH_BUFFER_BIT);
    BindProgram(GetDefaultProgram());
    SetModelViewProjectionMatrix(GetDefaultProgram(), &mvpMatrix);
    DrawGraphicsObjects();
    DrawPlayer();
    EndRender();

    // Render HUD
    glClear(GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    */
}
