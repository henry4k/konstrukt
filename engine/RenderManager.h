#ifndef __APOAPSIS_RENDER_MANAGER__
#define __APOAPSIS_RENDER_MANAGER__

#include "Math.h"


struct Solid;


bool InitRenderManager();
void DestroyRenderManager();
void RenderScene();

void SetCameraAttachmentTarget( Solid* target );
void SetCameraViewTransformation( glm::mat4 transformation );
void SetCameraFieldOfView( float fov );

#endif
