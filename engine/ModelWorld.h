#ifndef __KONSTRUKT_MODEL_WORLD__
#define __KONSTRUKT_MODEL_WORLD__

#include "Math.h"


struct Mesh;
struct ShaderProgram;
struct ShaderProgramSet;
struct ShaderVariableSet;
struct AttachmentTarget;
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

void DrawModelWorld( ModelWorld* world,
                     const ShaderProgramSet* programSet,
                     Camera* camera );


Model* CreateModel( ModelWorld* world );

void ReferenceModel( Model* model );
void ReleaseModel( Model* model );

void SetModelAttachmentTarget( Model* model, const AttachmentTarget* target );
void SetModelTransformation( Model* model, Mat4 transformation );
void SetModelOverlayLevel( Model* model, int level );
void SetModelMesh( Model* model, Mesh* mesh );
void SetModelProgramFamilyList( Model* model, const char* familyList );
ShaderVariableSet* GetModelShaderVariableSet( const Model* model );

#endif
