#ifndef __APOAPSIS_RENDER_MANAGER__
#define __APOAPSIS_RENDER_MANAGER__

#include "Math.h"
#include "Shader.h"


struct Texture;
struct Mesh;
struct Model;
struct Solid;


bool InitRenderManager();
void DestroyRenderManager();
void DrawModels( glm::mat4 mvpMatrix );

Model* CreateModel( ShaderProgram* program );

void ReferenceModel( Model* model );
void ReleaseModel( Model* model );

void SetModelAttachmentTarget( Model* model, Solid* target );
void SetModelTransformation( Model* model, glm::mat4 transformation );
void SetModelMesh( Model* model, Mesh* mesh );
void SetModelTexture( Model* model, Texture* texture );
void SetModelUniform( Model* model, const char* name, UniformValue* value );
void UnsetModelUniform( Model* model, const char* name );

#endif
