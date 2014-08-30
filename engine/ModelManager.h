#ifndef __APOAPSIS_MODEL_MANAGER__
#define __APOAPSIS_MODEL_MANAGER__

#include "Math.h"


struct Texture;
struct Mesh;
struct Model;
struct ShaderProgram;
struct Solid;
struct UniformValue;


enum ModelStage
{
    WORLD_STAGE,
    BACKGROUND_STAGE,
    HUD_STAGE,
    STAGE_COUNT
};


bool InitModelManager();
void DestroyModelManager();
void DrawModels( const glm::mat4* projectionTransformation,
                 const glm::mat4* viewTransformation,
                 const glm::mat4* modelTransformation );

Model* CreateModel( ModelStage stage, ShaderProgram* program );

void ReferenceModel( Model* model );
void ReleaseModel( Model* model );

void SetModelAttachmentTarget( Model* model, Solid* target );
void SetModelTransformation( Model* model, glm::mat4 transformation );
void SetModelMesh( Model* model, Mesh* mesh );
void SetModelTexture( Model* model, Texture* texture );
void SetModelUniform( Model* model, const char* name, UniformValue* value );
void UnsetModelUniform( Model* model, const char* name );

#endif
