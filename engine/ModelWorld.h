#ifndef __APOAPSIS_MODEL_WORLD__
#define __APOAPSIS_MODEL_WORLD__

#include "Math.h"
#include "Shader.h" // UniformType


struct Texture;
struct Mesh;
struct ShaderProgram;
struct ShaderProgramSet;
struct Solid;
struct Camera;


/**
 * A model world defines a possibly large set of models that can be rendered
 * together.
 *
 * @see Model
 * @see RenderLayer
 */
struct ModelWorld;

/**
 * @class RenderLayer
 * When rendering the model world, the models are grouped into layers.
 *
 * Models from a lower layer can't occlude models from higher layers.
 * Also each layer has own z near and z far planes.
 *
 * Render layers are used to separate HUD and background from the regular
 * scene.
 */

/**
 * Models are aggregations of meshes, textures, and shaders that can be used
 * to render something on the screen.
 */
struct Model;


ModelWorld* CreateModelWorld();

void ReferenceModelWorld( ModelWorld* world );
void ReleaseModelWorld( ModelWorld* world );

void SetRenderLayerNearAndFarPlanes( ModelWorld* world,
                                     int index,
                                     float zNear,
                                     float zFar );

void DrawModelWorld( const ModelWorld* world,
                     const ShaderProgramSet* programSet,
                     Camera* camera );


Model* CreateModel( ModelWorld* world, int renderLayerIndex );

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
