#include <string.h> // memset

#include "Math.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include "Reference.h"
#include "ModelWorld.h"
#include "PhysicsManager.h"
#include "Shader.h"
#include "Camera.h"

using glm::mat4;


struct Camera
{
    ReferenceCounter refCounter;
    ModelWorld* world;
    Solid* attachmentTarget;
    int attachmentFlags;
    glm::mat4 modelTransformation;
    glm::mat4 viewTransformation;

    float aspect;
    float zNear, zFar;
    bool projectionTransformationNeedsUpdate;
    glm::mat4 projectionTransformation;

    CameraProjectionType projectionType;
    float fieldOfView; // perspective projection
    float scale; // orthographic projection
};


Camera* CreateCamera( ModelWorld* world )
{
    Camera* camera = new Camera;
    memset(camera, 0, sizeof(Camera));

    InitReferenceCounter(&camera->refCounter);
    camera->world = world;
    ReferenceModelWorld(world);

    camera->modelTransformation = mat4(1);
    camera->viewTransformation = mat4(1);

    camera->aspect = 1;
    camera->zNear  = 0.1f;
    camera->zFar   = 100;

    camera->projectionType = CAMERA_PERSPECTIVE_PROJECTION;
    camera->fieldOfView = 80;
    camera->scale = 1;
    camera->projectionTransformationNeedsUpdate = true;

    return camera;
}

static void FreeCamera( Camera* camera )
{
    ReleaseModelWorld(camera->world);
    if(camera->attachmentTarget)
        ReleaseSolid(camera->attachmentTarget);
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

void SetCameraAttachmentTarget( Camera* camera, Solid* target, int flags )
{
    if(camera->attachmentTarget)
        ReleaseSolid(camera->attachmentTarget);
    camera->attachmentTarget = target;
    camera->attachmentFlags = flags;
    if(camera->attachmentTarget)
        ReferenceSolid(camera->attachmentTarget);
}

void SetCameraModelTransformation( Camera* camera, glm::mat4 transformation )
{
    camera->modelTransformation = transformation;
}

void SetCameraViewTransformation( Camera* camera, mat4 transformation )
{
    camera->viewTransformation = transformation;
}

void SetCameraAspect( Camera* camera, float aspect )
{
    assert(aspect > 0);
    camera->aspect = aspect;
    camera->projectionTransformationNeedsUpdate = true;
}

void SetCameraNearAndFarPlanes( Camera* camera, float zNear, float zFar )
{
    assert(zNear > 0);
    assert(zNear <= zFar);
    camera->zNear = zNear;
    camera->zFar = zFar;
    camera->projectionTransformationNeedsUpdate = true;
}

void SetCameraProjectionType( Camera* camera, CameraProjectionType type )
{
    camera->projectionType = type;
    camera->projectionTransformationNeedsUpdate = true;
}

void SetCameraFieldOfView( Camera* camera, float fov )
{
    assert(fov > 0);
    camera->fieldOfView = fov;
    camera->projectionTransformationNeedsUpdate = true;
}

void SetCameraScale( Camera* camera, float scale )
{
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
                    glm::perspective(camera->fieldOfView,
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
                    glm::ortho(halfWidth, // left
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

static const mat4 GetCameraModelTransformation( const Camera* camera )
{
    mat4 solidTransformation;
    if(camera->attachmentTarget)
        GetSolidTransformation(camera->attachmentTarget,
                               camera->attachmentFlags,
                               &solidTransformation);
    return solidTransformation * camera->modelTransformation;
}

template<class CalcFn>
static void CalcUniform( ShaderProgram* program,
                         const char* name,
                         CalcFn calcFn )
{
    if(HasUniform(program, name))
    {
        const mat4 value = calcFn();
        SetUniform(program,
                   name,
                   MAT4_UNIFORM,
                   (const UniformValue*)&value);
    }
}

void SetCameraUniforms( Camera* camera, ShaderProgram* program )
{
    UpdateCameraProjection(camera);
    const mat4* projection = &camera->projectionTransformation;

    SetUniform(program,
               "Projection",
               MAT4_UNIFORM,
               (const UniformValue*)projection);

    CalcUniform(program, "InverseProjection", [&]{
        return glm::inverse(*projection);
    });

    CalcUniform(program, "InverseTransposeProjection", [&]{
        return glm::inverseTranspose(*projection);
    });
}

void SetCameraModelUniforms( Camera* camera,
                             ShaderProgram* program,
                             const mat4* modelTransformation )
{
    UpdateCameraProjection(camera);
    const mat4* projection = &camera->projectionTransformation;
    const mat4* view = &camera->viewTransformation;
    const mat4  model = GetCameraModelTransformation(camera) *
                        *modelTransformation;

    const mat4 modelView = *view * model;
    const mat4 modelViewProjection = *projection * modelView;

    // These are almost always used:
    SetUniform(program,
               "ModelView",
               MAT4_UNIFORM,
               (const UniformValue*)&modelView);
    SetUniform(program,
               "ModelViewProjection",
               MAT4_UNIFORM,
               (const UniformValue*)&modelViewProjection);

    // Same set, but inversed:
    CalcUniform(program, "InverseModelView", [&]{
        return glm::inverse(modelView);
    });
    CalcUniform(program, "InverseModelViewProjection", [&]{
        return glm::inverse(modelViewProjection);
    });

    // Same set, but inversed and transposed:
    CalcUniform(program, "InverseTransposeModelView", [&]{
        return glm::inverseTranspose(modelView);
    });
    CalcUniform(program, "InverseTransposeModelViewProjection", [&]{
        return glm::inverseTranspose(modelViewProjection);
    });
}

void DrawCameraView( Camera* camera, ShaderProgramSet* set )
{
    DrawModelWorld(camera->world, set, camera);
}
