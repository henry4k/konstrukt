#include "Math.h"
#include <glm/gtc/matrix_transform.hpp>
#include "OpenGL.h"
#include "Vertex.h"
#include "Window.h"
#include "Reference.h"
#include "PhysicsManager.h"
#include "ModelManager.h"
#include "RenderManager.h"


static float CameraFieldOfView = 90;
static Solid* CameraAttachmentTarget = NULL;
static glm::mat4 CameraViewTransformation;
static glm::mat4 CameraProjectionTransformation;

static void UpdateProjectionTransformation();
static void OnFramebufferResize( int width, int height );

bool InitRenderManager()
{
    CameraAttachmentTarget = NULL;

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
    SetCameraAttachmentTarget(NULL);
}

void SetCameraAttachmentTarget( Solid* target )
{
    if(CameraAttachmentTarget)
        ReleaseSolid(CameraAttachmentTarget);
    CameraAttachmentTarget = target;
    if(CameraAttachmentTarget)
        ReferenceSolid(CameraAttachmentTarget);
}

void SetCameraViewTransformation( glm::mat4 transformation )
{
    CameraViewTransformation = transformation;
}

void SetCameraFieldOfView( float fov )
{
    CameraFieldOfView = fov;
    UpdateProjectionTransformation();
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
    const float aspect = float(framebufferSize[0]) / float(framebufferSize[1]);
    CameraProjectionTransformation = perspective(CameraFieldOfView,
                                                 aspect,
                                                 0.1f, 100.0f);
}

void RenderScene()
{
    glm::mat4 cameraTargetTransformation;
    if(CameraAttachmentTarget)
        GetSolidTransformation(CameraAttachmentTarget, &cameraTargetTransformation);

    const glm::mat4 cameraTransformation = CameraProjectionTransformation *
                                           CameraViewTransformation *
                                           cameraTargetTransformation;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    DrawModels(&cameraTransformation);
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
