#include "Math.h"
#include <glm/gtc/matrix_transform.hpp>
#include "OpenGL.h"
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


bool InitRenderManager()
{
    EnableVertexArrays();
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    //glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
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
    UniformValue timeValue;
    timeValue.f = GetTime();
    SetGlobalUniform("Time", FLOAT_UNIFORM, &timeValue);

    RenderTarget* defaultRenderTarget = GetDefaultRenderTarget();
    UpdateRenderTarget(defaultRenderTarget);

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
