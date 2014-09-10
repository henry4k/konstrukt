#include <string.h> // memset

#include "OpenGL.h"
#include "Reference.h"
#include "Texture.h"
#include "Camera.h"
#include "Shader.h"
#include "RenderTarget.h"


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
            Gluint handle;
            Texture* texture;
        } fbo;
    } typeSpecific;
};


static RenderTarget DefaultRenderTarget;

static void InitDefaultRenderTarget()
{
    RenderTarget* target = &DefaultRendertarget;
    memset(target, 0, sizeof(RenderTarget));
    InitReferenceCounter(&target->refCounter);
    target->type = DEFAULT_RENDER_TARGET;
    return true;
}

RenderTarget* GetDefaultRenderTarget()
{
    static bool unused = InitDefaultRenderTarget();
    return &DefaultRenderTarget;
}

RenderTarget* CreateTextureRenderTarget( Camera* camera,
                                         ShaderProgramSet* shaderProgramSet,
                                         Texture* texture )
{
    RenderTarget* target = new RenderTarget;
    memset(target, 0, sizeof(RenderTarget));
    InitReferenceCounter(&target->refCounter);

    target->camera = camera;
    target->shaderProgramSet = shaderProgramSet;

    ReferenceCamera(camera);
    ReferenceShaderProgramSet(shaderProgramSet);

    target->type = FBO_RENDER_TARGET;
    target->typeSpecific.fbo.handle = 0; // TODO
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
