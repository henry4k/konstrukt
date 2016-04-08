#ifndef __KONSTRUKT_CAMERA__
#define __KONSTRUKT_CAMERA__

#include "Math.h"


struct ModelWorld;
struct LightWorld;
struct Solid;
struct Camera;
struct ShaderProgram;
struct ShaderProgramSet;
struct ShaderVariableSet;

enum CameraProjectionType
{
    CAMERA_PERSPECTIVE_PROJECTION,
    CAMERA_ORTHOGRAPHIC_PROJECTION
};


/**
 * @param lightWorld
 * Optional! May be `NULL`.
 */
Camera* CreateCamera( ModelWorld* modelWorld,
                      LightWorld* lightWorld );

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

ShaderVariableSet* GetCameraShaderVariableSet( const Camera* camera );
LightWorld* GetCameraLightWorld( const Camera* camera );
void GenerateCameraModelShaderVariables( const Camera* camera,
                                         ShaderVariableSet* variableSet,
                                         const ShaderProgram* program,
                                         Mat4 modelTransformation,
                                         float modelRadius );

void DrawCameraView( Camera* camera, ShaderProgramSet* set );

#endif
