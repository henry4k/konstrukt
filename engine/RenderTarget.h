#ifndef __KONSTRUKT_RENDER_TARGET__
#define __KONSTRUKT_RENDER_TARGET__

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

/**
 * A render target can use multiple cameras.
 *
 * Geometry of cameras with a lower layer can't occlude geometry from higher
 * layers.  This can be used to separate HUD and background from the regular
 * scene.
 */
void SetRenderTargetCamera( RenderTarget* target, Camera* camera, int layer );
void SetRenderTargetShaderProgramSet( RenderTarget* target, ShaderProgramSet* set );

Camera* GetRenderTargetCamera( RenderTarget* target, int layer );
ShaderProgramSet* GetRenderTargetShaderProgramSet( RenderTarget* target );

/**
 * 
 */
void UpdateRenderTarget( RenderTarget* target );

#endif
