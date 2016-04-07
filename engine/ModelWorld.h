#ifndef __KONSTRUKT_MODEL_WORLD__
#define __KONSTRUKT_MODEL_WORLD__

#include "Math.h"
#include "Shader.h" // UniformType


struct Texture;
struct Mesh;
struct ShaderProgram;
struct ShaderProgramSet;
struct UniformSet;
struct Solid;
struct Camera;


/**
 * A model world defines a possibly large set of models that can be rendered
 * together.
 *
 * @see Model
 */
struct ModelWorld;

/**
 * Models are aggregations of meshes, textures, and shaders that can be used
 * to render something on the screen.
 */
struct Model;


ModelWorld* CreateModelWorld();

void ReferenceModelWorld( ModelWorld* world );
void ReleaseModelWorld( ModelWorld* world );

void DrawModelWorld( const ModelWorld* world,
                     const ShaderProgramSet* programSet,
                     Camera* camera );


Model* CreateModel( ModelWorld* world );

void ReferenceModel( Model* model );
void ReleaseModel( Model* model );

void SetModelAttachmentTarget( Model* model, Solid* target, int flags );
void SetModelTransformation( Model* model, Mat4 transformation );
void SetModelOverlayLevel( Model* model, int level );
void SetModelMesh( Model* model, Mesh* mesh );
void SetModelTexture( Model* model, int unit, Texture* texture );
void SetModelProgramFamilyList( Model* model, const char* familyList );
UniformSet* GetModelUniformSet( Model* model );
void SetModelUniform( Model* model,
                      const char* name,
                      UniformType type,
                      const UniformValue* value );
void UnsetModelUniform( Model* model, const char* name );

#endif
