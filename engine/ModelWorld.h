#ifndef __APOAPSIS_MODEL_WORLD__
#define __APOAPSIS_MODEL_WORLD__

#include "Math.h"
#include "Shader.h" // UniformType


struct Texture;
struct Mesh;
struct Model;
struct ShaderProgram;
struct ShaderProgramSet;
struct Solid;


struct ModelWorld;

ModelWorld* CreateModelWorld();

void ReferenceModelWorld( ModelWorld* world );
void ReleaseModelWorld( ModelWorld* world );

void DrawModelWorld( const ModelWorld* world,
                     const ShaderProgramSet* programSet,
                     const glm::mat4* projectionTransformation,
                     const glm::mat4* viewTransformation,
                     const glm::mat4* modelTransformation );


Model* CreateModel( ModelWorld* world );

void ReferenceModel( Model* model );
void ReleaseModel( Model* model );

void SetModelAttachmentTarget( Model* model, Solid* target );
void SetModelTransformation( Model* model, glm::mat4 transformation );
void SetModelMesh( Model* model, Mesh* mesh );
void SetModelTexture( Model* model, int unit, Texture* texture );
void SetModelProgramFamilyList( Model* model, const char* familyList );
void SetModelUniform( Model* model,
                      const char* name,
                      UniformType type,
                      const UniformValue* value );
void UnsetModelUniform( Model* model, const char* name );

#endif
