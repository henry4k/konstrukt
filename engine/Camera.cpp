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
    glm::mat4 viewTransformation;
    glm::mat4 projectionTransformation;
    float fieldOfView;
};


Camera* CreateCamera( ModelWorld* world )
{
    Camera* camera = new Camera;
    memset(camera, 0, sizeof(Camera));

    InitReferenceCounter(&camera->refCounter);
    camera->world = world;
    ReferenceModelWorld(world);
    camera->viewTransformation = mat4();
    camera->projectionTransformation = mat4();
    camera->fieldOfView = 10;

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

void SetCameraAttachmentTarget( Camera* camera, Solid* target )
{
    if(camera->attachmentTarget)
        ReleaseSolid(camera->attachmentTarget);
    camera->attachmentTarget = target;
    if(camera->attachmentTarget)
        ReferenceSolid(camera->attachmentTarget);
}

void SetCameraViewTransformation( Camera* camera, mat4 transformation )
{
    camera->viewTransformation = transformation;
}

void SetCameraFieldOfView( Camera* camera, float fov )
{
    camera->fieldOfView = fov;
}

void UpdateCameraProjection( Camera* camera, glm::ivec2 framebufferSize )
{
    const float aspect = float(framebufferSize[0]) / float(framebufferSize[1]);
    camera->projectionTransformation = glm::perspective(camera->fieldOfView,
                                                        aspect,
                                                        0.1f, 100.0f);
}

static const mat4 GetCameraModelTransformation( const Camera* camera )
{
    mat4 solidTransformation;
    GetSolidTransformation(camera->attachmentTarget, &solidTransformation);
    return solidTransformation;
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

void SetCameraUniforms( const Camera* camera, ShaderProgram* program )
{
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

void SetCameraModelUniforms( const Camera* camera,
                             ShaderProgram* program,
                             const mat4* modelTransformation )
{
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

void DrawCameraView( const Camera* camera, ShaderProgramSet* set )
{
    DrawModelWorld(camera->world, set, camera);
}
