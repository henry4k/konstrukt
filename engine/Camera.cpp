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

    float fieldOfView;
    float aspect;
    float zNear, zFar;
    bool projectionTransformationNeedsUpdate;
    glm::mat4 projectionTransformation;
};


Camera* CreateCamera( ModelWorld* world )
{
    Camera* camera = new Camera;
    memset(camera, 0, sizeof(Camera));

    InitReferenceCounter(&camera->refCounter);
    camera->world = world;
    ReferenceModelWorld(world);

    camera->viewTransformation = mat4();

    camera->fieldOfView = 80;
    camera->aspect = 1;
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

void SetCameraFieldOfView( Camera* camera, float fov )
{
    assert(fov > 0);
    camera->fieldOfView = fov;
    camera->projectionTransformationNeedsUpdate = true;
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

static void UpdateCameraProjection( Camera* camera )
{
    if(camera->projectionTransformationNeedsUpdate)
    {
        camera->projectionTransformation = glm::perspective(camera->fieldOfView,
                                                            camera->aspect,
                                                            camera->zNear,
                                                            camera->zFar);
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
