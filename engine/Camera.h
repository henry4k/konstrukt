#ifndef __APOAPSIS_CAMERA__
#define __APOAPSIS_CAMERA__

#include "Math.h"


struct ModelWorld;
struct Solid;
struct Camera;
struct ShaderProgram;
struct ShaderProgramSet;


Camera* CreateCamera( ModelWorld* world );

void ReferenceCamera( Camera* camera );
void ReleaseCamera( Camera* camera );

void SetCameraAttachmentTarget( Camera* camera, Solid* target );
void SetCameraViewTransformation( Camera* camera, glm::mat4 transformation );
void SetCameraFieldOfView( Camera* camera, float fov );

void UpdateCameraProjection( Camera* camera, glm::ivec2 framebufferSize );

/**
 * Sets uniform values, that depend solely on the camera.
 *
 * These are:
 * - mat4 Projection
 * - mat4 InverseProjection
 * - mat4 InverseTransposeProjection
 */
void SetCameraUniforms( const Camera* camera, ShaderProgram* program );

/**
 * Sets uniform values, that depend on camera and model transformation.
 *
 * These are:
 * - mat4 ModelView
 * - mat4 ModelViewProjection
 * - mat4 InverseModelView
 * - mat4 InverseModelViewProjection
 * - mat4 InverseTransposeModelView
 * - mat4 InverseTransposeModelViewProjection
 */
void SetCameraModelUniforms( const Camera* camera,
                             ShaderProgram* program,
                             const glm::mat4* modelTransformation );

void DrawCameraView( const Camera* camera, ShaderProgramSet* set );

#endif
