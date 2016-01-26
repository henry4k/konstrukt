#include <assert.h>
#include <string.h> // memset

#include "Common.h"
#include "OpenGL.h"
#include "Reference.h"
#include "Texture.h"
#include "Camera.h"
#include "Shader.h"
#include "Window.h" // SetFramebufferFn
#include "RenderTarget.h"


static const GLuint INVALID_FBO = 0;
static const int MAX_CAMERAS = 8;

enum RenderTargetType
{
    DEFAULT_RENDER_TARGET,
    FBO_RENDER_TARGET
};

struct RenderTarget
{
    ReferenceCounter refCounter;
    Camera* cameras[MAX_CAMERAS];
    ShaderProgramSet* shaderProgramSet;

    RenderTargetType type;
    union
    {
        struct
        {
            GLuint handle;
            Texture* texture;
        } fbo;
    } typeSpecific;
};


static RenderTarget* DefaultRenderTarget = NULL;


static void FreeRenderTarget( RenderTarget* target );
static bool RenderTargetIsComplete( const RenderTarget* target );
static void OnFramebufferResize( int width, int height );

bool InitDefaultRenderTarget()
{
    RenderTarget* target = DefaultRenderTarget = new RenderTarget;
    memset(target, 0, sizeof(RenderTarget));
    InitReferenceCounter(&target->refCounter);
    target->type = DEFAULT_RENDER_TARGET;

    SetFrambufferFn(OnFramebufferResize);

    return true;
}

void DestroyDefaultRenderTarget()
{
    FreeRenderTarget(DefaultRenderTarget);
    DefaultRenderTarget = NULL;
}

RenderTarget* GetDefaultRenderTarget()
{
    return DefaultRenderTarget;
}

RenderTarget* CreateTextureRenderTarget( Texture* texture )
{
    RenderTarget* target = new RenderTarget;
    memset(target, 0, sizeof(RenderTarget));
    InitReferenceCounter(&target->refCounter);

    target->type = FBO_RENDER_TARGET;
    target->typeSpecific.fbo.handle = INVALID_FBO; // TODO
    target->typeSpecific.fbo.texture = texture;
    ReferenceTexture(texture);

    return target;
}

static void FreeRenderTarget( RenderTarget* target )
{
    for(int i = 0; i < MAX_CAMERAS; i++)
    {
        Camera* camera = target->cameras[i];
        if(camera)
            ReleaseCamera(camera);
    }

    if(target->shaderProgramSet)
        ReleaseShaderProgramSet(target->shaderProgramSet);

    switch(target->type)
    {
        case DEFAULT_RENDER_TARGET:
            break;

        case FBO_RENDER_TARGET:
            // destroy fbo
            if(target->typeSpecific.fbo.texture)
                ReleaseTexture(target->typeSpecific.fbo.texture);
            break;
    }

    delete target;
}

void ReferenceRenderTarget( RenderTarget* target )
{
    Reference(&target->refCounter);
}

void ReleaseRenderTarget( RenderTarget* target )
{
    Release(&target->refCounter);
    if(!HasReferences(&target->refCounter))
        FreeRenderTarget(target);
}

static void UpdateDefaultRenderTargetCameraProjection()
{
    int width, height;
    GetFramebufferSize(&width, &height);
    const float aspect = (float)width / (float)height;

    RenderTarget* renderTarget = GetDefaultRenderTarget();
    for(int i = 0; i < MAX_CAMERAS; i++)
    {
        Camera* camera = renderTarget->cameras[i];
        if(camera)
            SetCameraAspect(camera, aspect);
    }
}

static void OnFramebufferResize( int width, int height )
{
    glViewport(0, 0, width, height);
    UpdateDefaultRenderTargetCameraProjection();
}

void SetRenderTargetCamera( RenderTarget* target, Camera* camera, int layer )
{
    assert(layer >= 0 && layer < MAX_CAMERAS);

    Camera* oldCamera = target->cameras[layer];
    if(oldCamera)
        ReleaseCamera(oldCamera);
    target->cameras[layer] = camera;
    if(camera)
    {
        ReferenceCamera(camera);
        if(target == DefaultRenderTarget)
            UpdateDefaultRenderTargetCameraProjection();
    }
}

void SetRenderTargetShaderProgramSet( RenderTarget* target, ShaderProgramSet* set )
{
    if(target->shaderProgramSet)
        ReleaseShaderProgramSet(target->shaderProgramSet);
    target->shaderProgramSet = set;
    if(target->shaderProgramSet)
        ReferenceShaderProgramSet(target->shaderProgramSet);
}

Camera* GetRenderTargetCamera( RenderTarget* target, int layer )
{
    assert(layer >= 0 && layer < MAX_CAMERAS);
    return target->cameras[layer];
}

ShaderProgramSet* GetRenderTargetShaderProgramSet( RenderTarget* target )
{
    return target->shaderProgramSet;
}

static bool RenderTargetIsComplete( const RenderTarget* target )
{
    if(!target->shaderProgramSet)
        return false;

    switch(target->type)
    {
        case DEFAULT_RENDER_TARGET:
            break;

        case FBO_RENDER_TARGET:
            if(target->typeSpecific.fbo.handle == INVALID_FBO ||
              !target->typeSpecific.fbo.texture)
                return false;
            break;
    }

    return true;
}

void UpdateRenderTarget( RenderTarget* target )
{
    if(!RenderTargetIsComplete(target))
    {
        Error("Can't update incomplete render target %p.", target);
        return;
    }

    switch(target->type)
    {
        case DEFAULT_RENDER_TARGET:
            break;

        case FBO_RENDER_TARGET:
            break;
    }

    for(int i = 0; i < MAX_CAMERAS; i++)
    {
        Camera* camera = target->cameras[i];
        if(camera)
        {
            DrawCameraView(camera, target->shaderProgramSet);
            glClear(GL_DEPTH_BUFFER_BIT);
        }
    }
}
