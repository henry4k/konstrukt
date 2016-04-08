#include "Math.h"
#include "OpenGL.h" // glfwGetTime
#include "Vertex.h"
#include "Reference.h"
#include "PhysicsManager.h"
#include "ModelWorld.h"
#include "Camera.h"
#include "Time.h"
#include "Shader.h"
#include "RenderTarget.h"
#include "Window.h" // SwapBuffers
#include "RenderManager.h"


static double LastFrameTimestamp;
static double FrameTime;


bool InitRenderManager()
{
    EnableVertexArrays();
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
    glBlendFuncSeparate(GL_ONE, GL_ONE_MINUS_SRC_ALPHA, // RGB
                        GL_ONE, GL_ZERO);               // A
    return true;
}

void DestroyRenderManager()
{
}

void RenderScene()
{
    SetFloatUniform(GetGlobalShaderVariableSet(), "Time", GetTime());

    RenderTarget* defaultRenderTarget = GetDefaultRenderTarget();
    UpdateRenderTarget(defaultRenderTarget);

    SwapBuffers();
    const double curTimestamp = glfwGetTime();
    FrameTime = curTimestamp - LastFrameTimestamp;
    LastFrameTimestamp = curTimestamp;

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

double GetFrameTime()
{
    return FrameTime;
}
