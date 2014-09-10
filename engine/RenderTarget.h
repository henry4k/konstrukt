#ifndef __APOAPSIS_RENDER_TARGET__
#define __APOAPSIS_RENDER_TARGET__

struct Camera;
struct ShaderProgramSet;
struct Texture;


/**
 * TODO: Maybe call them just Framebuffers, cause thats what they are.
 */
struct RenderTarget;


/**
 * Provides the window-system created render target.
 */
RenderTarget* GetDefaultRenderTarget();

RenderTarget* CreateTextureRenderTarget( Camera* camera,
                                         ShaderProgramSet* shaderProgramSet,
                                         Texture* texture );

void ReferenceRenderTarget( RenderTarget* target );
void ReleaseRenderTarget( RenderTarget* target );

#endif
