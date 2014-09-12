#ifndef __APOAPSIS_RENDER_TARGET__
#define __APOAPSIS_RENDER_TARGET__

struct Camera;
struct ShaderProgramSet;
struct Texture;


/**
 * TODO: Maybe call them just Framebuffers, cause thats what they are.
 */
struct RenderTarget;


bool InitDefaultRenderTarget();
void DestroyDefaultRenderTarget();

/**
 * Provides the window-system created render target.
 */
RenderTarget* GetDefaultRenderTarget();
RenderTarget* CreateTextureRenderTarget( Texture* texture );

void ReferenceRenderTarget( RenderTarget* target );
void ReleaseRenderTarget( RenderTarget* target );

void SetRenderTargetCamera( RenderTarget* target, Camera* camera );
void SetRenderTargetShaderProgramSet( RenderTarget* target, ShaderProgramSet* set );

Camera* GetRenderTargetCamera( RenderTarget* target );
ShaderProgramSet* GetRenderTargetShaderProgramSet( RenderTarget* target );

/**
 * 
 */
void UpdateRenderTarget( RenderTarget* target );

#endif
