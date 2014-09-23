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

enum RenderTargetType
{
    DEFAULT_RENDER_TARGET,
    FBO_RENDER_TARGET
};

struct RenderTarget
{
    ReferenceCounter refCounter;
    Camera* camera;
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
    ReleaseCamera(target->camera);
    ReleaseShaderProgramSet(target->shaderProgramSet);

    switch(target->type)
    {
        case DEFAULT_RENDER_TARGET:
            break;

        case FBO_RENDER_TARGET:
            // destroy fbo
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
    using namespace glm;
    const ivec2 framebufferSize = GetFramebufferSize();

    RenderTarget* renderTarget = GetDefaultRenderTarget();
    Camera* camera = GetRenderTargetCamera(renderTarget);
    if(camera)
    {
        const float aspect = float(framebufferSize[0]) /
                             float(framebufferSize[1]);
        SetCameraAspect(camera, aspect);
    }
}

static void OnFramebufferResize( int width, int height )
{
    glViewport(0, 0, width, height);
    UpdateDefaultRenderTargetCameraProjection();
}

void SetRenderTargetCamera( RenderTarget* target, Camera* camera )
{
    if(target->camera)
        ReleaseCamera(target->camera);
    target->camera = camera;
    if(target->camera)
    {
        ReferenceCamera(target->camera);
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

Camera* GetRenderTargetCamera( RenderTarget* target )
{
    return target->camera;
}

ShaderProgramSet* GetRenderTargetShaderProgramSet( RenderTarget* target )
{
    return target->shaderProgramSet;
}

static bool RenderTargetIsComplete( const RenderTarget* target )
{
    if(!target->camera || !target->shaderProgramSet)
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

    DrawCameraView(target->camera, target->shaderProgramSet);
}
