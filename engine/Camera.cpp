#include <assert.h>
#include <string.h> // memset

#include "Common.h"
#include "Math.h"
#include "Reference.h"
#include "ModelWorld.h"
#include "LightWorld.h"
#include "AttachmentTarget.h"
#include "Shader.h"
#include "Camera.h"


struct Camera
{
    ReferenceCounter refCounter;
    ModelWorld* modelWorld;
    LightWorld* lightWorld;
    ShaderVariableSet* shaderVariableSet;
    AttachmentTarget attachmentTarget;
    Mat4 modelTransformation;
    Mat4 viewTransformation;

    float aspect;
    float zNear, zFar;
    bool projectionTransformationNeedsUpdate;
    Mat4 projectionTransformation;

    CameraProjectionType projectionType;
    float fieldOfView; // perspective projection
    float scale; // orthographic projection
};


Camera* CreateCamera( ModelWorld* modelWorld,
                      LightWorld* lightWorld )
{
    assert(InSerialPhase());

    Camera* camera = new Camera;
    memset(camera, 0, sizeof(Camera));

    InitReferenceCounter(&camera->refCounter);
    camera->modelWorld = modelWorld;
    ReferenceModelWorld(modelWorld);
    camera->lightWorld = lightWorld;
    if(camera->lightWorld)
        ReferenceLightWorld(lightWorld);
    camera->shaderVariableSet = CreateShaderVariableSet();
    InitAttachmentTarget(&camera->attachmentTarget);

    camera->modelTransformation = Mat4Identity;
    camera->viewTransformation  = Mat4Identity;

    camera->aspect = 1;
    camera->zNear  = 0.1f;
    camera->zFar   = 100;

    camera->projectionType = CAMERA_PERSPECTIVE_PROJECTION;
    camera->fieldOfView = TAU/4.0; // 90 degrees
    camera->scale = 1;
    camera->projectionTransformationNeedsUpdate = true;

    return camera;
}

static void FreeCamera( Camera* camera )
{
    assert(InSerialPhase());
    ReleaseModelWorld(camera->modelWorld);
    if(camera->lightWorld)
        ReleaseLightWorld(camera->lightWorld);
    DestroyAttachmentTarget(&camera->attachmentTarget);
    FreeShaderVariableSet(camera->shaderVariableSet);
    delete camera;
}

void ReferenceCamera( Camera* camera )
{
    Reference(&camera->refCounter);
}

void ReleaseCamera( Camera* camera )
{
    Release(&camera->refCounter);
    if(!HasReferences(&camera->refCounter))
        FreeCamera(camera);
}

void SetCameraAttachmentTarget( Camera* camera, const AttachmentTarget* target )
{
    assert(InSerialPhase());
    CopyAttachmentTarget(&camera->attachmentTarget, target);
}

void SetCameraModelTransformation( Camera* camera, Mat4 transformation )
{
    assert(InSerialPhase());
    camera->modelTransformation = transformation;
}

void SetCameraViewTransformation( Camera* camera, Mat4 transformation )
{
    assert(InSerialPhase());
    camera->viewTransformation = transformation;
}

void SetCameraAspect( Camera* camera, float aspect )
{
    assert(InSerialPhase());
    assert(aspect > 0);
    camera->aspect = aspect;
    camera->projectionTransformationNeedsUpdate = true;
}

void SetCameraNearAndFarPlanes( Camera* camera, float zNear, float zFar )
{
    assert(InSerialPhase());
    assert(zNear > 0);
    assert(zNear <= zFar);
    camera->zNear = zNear;
    camera->zFar = zFar;
    camera->projectionTransformationNeedsUpdate = true;
}

void SetCameraProjectionType( Camera* camera, CameraProjectionType type )
{
    assert(InSerialPhase());
    camera->projectionType = type;
    camera->projectionTransformationNeedsUpdate = true;
}

void SetCameraFieldOfView( Camera* camera, float fov )
{
    assert(InSerialPhase());
    assert(fov > 0);
    camera->fieldOfView = fov;
    camera->projectionTransformationNeedsUpdate = true;
}

void SetCameraScale( Camera* camera, float scale )
{
    assert(InSerialPhase());
    assert(scale > 0);
    camera->scale = scale;
    camera->projectionTransformationNeedsUpdate = true;
}

static void UpdateCameraProjection( Camera* camera )
{
    if(camera->projectionTransformationNeedsUpdate)
    {
        switch(camera->projectionType)
        {
            case CAMERA_PERSPECTIVE_PROJECTION:
                camera->projectionTransformation =
                    PerspectivicProjection(camera->fieldOfView,
                                           camera->aspect,
                                           camera->zNear,
                                           camera->zFar);
                break;

            case CAMERA_ORTHOGRAPHIC_PROJECTION:
                const float aspect = camera->aspect;
                const float scale  = camera->scale;
                const float halfWidth  = aspect * scale * 0.5f;
                const float halfHeight =          scale * 0.5f;
                camera->projectionTransformation =
                    OrthographicProjection(halfWidth, // left
                                           halfWidth, // right
                                           halfHeight, // bottom
                                           halfHeight, // top
                                           camera->zNear,
                                           camera->zFar);
                break;
        }
        camera->projectionTransformationNeedsUpdate = false;
    }
}

static const Mat4 GetCameraModelTransformation( const Camera* camera )
{
    const Mat4 t =
        GetAttachmentTargetTransformation(&camera->attachmentTarget);
    return MulMat4(InverseMat4(t),
                   camera->modelTransformation);
}

static void UpdateCameraShaderVariables( const Camera* camera )
{
    SetMat4Uniform(camera->shaderVariableSet,
                   "View",
                   camera->viewTransformation);

    const Mat4* projection = &camera->projectionTransformation;
    SetMat4Uniform(camera->shaderVariableSet,
                   "Projection",
                   *projection);
    SetMat4Uniform(camera->shaderVariableSet,
                   "InverseProjection",
                   InverseMat4(*projection));
    SetMat4Uniform(camera->shaderVariableSet,
                   "InverseTransposeProjection",
                   InverseMat4(TransposeMat4(*projection)));
}

ShaderVariableSet* GetCameraShaderVariableSet( const Camera* camera )
{
    return camera->shaderVariableSet;
}

LightWorld* GetCameraLightWorld( const Camera* camera )
{
    return camera->lightWorld;
}

void GenerateCameraModelShaderVariables( const Camera* camera,
                                         ShaderVariableSet* variableSet,
                                         const ShaderProgram* program,
                                         Mat4 modelTransformation,
                                         float modelRadius )
{
    const Mat4* projection = &camera->projectionTransformation;
    const Mat4* view       = &camera->viewTransformation;
    const Mat4  model      =
        MulMat4(GetCameraModelTransformation(camera), modelTransformation);

    const Mat4 modelView = MulMat4(*view, model);
    const Mat4 modelViewProjection = MulMat4(*projection, modelView);

    SetMat4Uniform(variableSet, "Model", modelTransformation); // TODO
    SetMat4Uniform(variableSet, "ModelView", modelView);
    SetMat4Uniform(variableSet, "ModelViewProjection", modelViewProjection);

#define CALC_MAT4(NAME, CALCULATION) \
    if(HasUniform(program, (NAME))) \
        SetMat4Uniform(variableSet, (NAME), (CALCULATION));
    // Same set, but inversed:
    CALC_MAT4("InverseModelView",
              InverseMat4(modelView));
    CALC_MAT4("InverseModelViewProjection",
              InverseMat4(modelViewProjection));

    // Same set, but inversed and transposed:
    CALC_MAT4("InverseTransposeModelView",
              InverseMat4(TransposeMat4(modelView)));
    CALC_MAT4("InverseTransposeModelViewProjection",
              InverseMat4(TransposeMat4(modelViewProjection)));
#undef CALC_MAT4

    if(camera->lightWorld)
    {
        const Vec3 position = MulMat4ByVec3(modelTransformation, Vec3Zero); // TODO
        GenerateLightShaderVariables(camera->lightWorld,
                                     variableSet,
                                     position,
                                     modelRadius);
    }
}

void DrawCameraView( Camera* camera, ShaderProgramSet* set )
{
    // TODO: Light world could be used by multiple cameras, so UpdateLights
    // could be called needlessly multiple times a frame.
    if(camera->lightWorld)
        UpdateLights(camera->lightWorld);

    // Dito:
    UpdateCameraProjection(camera);
    UpdateCameraShaderVariables(camera);

    DrawModelWorld(camera->modelWorld, set, camera);
}
