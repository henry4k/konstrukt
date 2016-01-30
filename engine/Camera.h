#ifndef __KONSTRUKT_CAMERA__
#define __KONSTRUKT_CAMERA__

#include "Math.h"


struct ModelWorld;
struct Solid;
struct Camera;
struct ShaderProgram;
struct ShaderProgramSet;

enum CameraProjectionType
{
    CAMERA_PERSPECTIVE_PROJECTION,
    CAMERA_ORTHOGRAPHIC_PROJECTION
};


Camera* CreateCamera( ModelWorld* world );

void ReferenceCamera( Camera* camera );
void ReleaseCamera( Camera* camera );

void SetCameraAttachmentTarget( Camera* camera, Solid* target, int flags);
void SetCameraModelTransformation( Camera* camera, Mat4 transformation );
void SetCameraViewTransformation( Camera* camera, Mat4 transformation );
void SetCameraAspect( Camera* camera, float aspect );
void SetCameraNearAndFarPlanes( Camera* camera, float zNear, float zFar );
void SetCameraProjectionType( Camera* camera, CameraProjectionType type );
void SetCameraFieldOfView( Camera* camera, float fov );
void SetCameraScale( Camera* camera, float scale );

/**
 * Sets uniform values, that depend solely on the camera.
 *
 * These are:
 * - mat4 Projection
 * - mat4 InverseProjection
 * - mat4 InverseTransposeProjection
 */
void SetCameraUniforms( Camera* camera, ShaderProgram* program );

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
void SetCameraModelUniforms( Camera* camera,
                             ShaderProgram* program,
                             const Mat4* modelTransformation );

void DrawCameraView( Camera* camera, ShaderProgramSet* set );

#endif
