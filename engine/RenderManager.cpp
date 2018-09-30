#include <assert.h>

#include "Common.h"
#include "Profiler.h"
#include "OpenGL.h" // glfwGetTime
#include "Vertex.h"
#include "Time.h"
#include "Shader.h"
#include "RenderTarget.h"
#include "Window.h" // SwapBuffers
#include "JobManager.h"
#include "RenderManager.h"


DefineCounter(FrameTimeCounter, "frame time (ms)");
static double LastFrameTimestamp;
static double FrameTime;
static JobId UpdateJob;


void InitRenderManager()
{
    assert(InSerialPhase());
    InitCounter(FrameTimeCounter);
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
}

void DestroyRenderManager()
{
    assert(InSerialPhase());
}

static void RenderScene( void* _data )
{
    ProfileScope("RenderScene");

    SetFloatUniform(GetGlobalShaderVariableSet(), "Time", GetTime());

    //RenderTarget* defaultRenderTarget = GetDefaultRenderTarget();
    //UpdateRenderTarget(defaultRenderTarget); // TODO

    SwapBuffers();
    const double curTimestamp = glfwGetTime();
    FrameTime = curTimestamp - LastFrameTimestamp;
    LastFrameTimestamp = curTimestamp;

    SetCounter(FrameTimeCounter, FrameTime*1000);

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

void BeginRenderManagerUpdate( void* _context, double _timeDelta )
{
    assert(InSerialPhase());
    UpdateJob = CreateJob({"RenderScene", RenderScene});
}

void CompleteRenderManagerUpdate( void* _context )
{
    assert(InSerialPhase());
    WaitForJobs(&UpdateJob, 1);
}
