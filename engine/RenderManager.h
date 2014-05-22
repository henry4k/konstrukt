#ifndef __APOAPSIS_RENDER_MANAGER__
#define __APOAPSIS_RENDER_MANAGER__

#include "Math.h"
#include "Texture.h"
#include "Shader.h"

struct Mesh;
struct Model;

bool InitRenderManager();
void DestroyRenderManager();
void DrawModels( glm::mat4 mvpMatrix );

Model* CreateModel();
void FreeModel( Model* model );
void SetModelTransformation( Model* model, glm::mat4 transformation );
void SetModelMesh( Model* model, Mesh* mesh );
void SetModelShaderProgram( Model* model, ShaderProgram program );

void SetModelFloatUniform( Model* model, const char* name, float value );
void SetModelVec3Uniform( Model* model, const char* name, glm::vec3 value );
void UnsetModelUniform( Model* model, const char* name );

#endif