#ifndef __APOAPSIS_RENDER_MANAGER__
#define __APOAPSIS_RENDER_MANAGER__

#include "Math.h"
#include "Shader.h"
#include "Texture.h"

struct Mesh;
struct Model;

bool InitRenderManager();
void DestroyRenderManager();
void DrawModels( glm::mat4 mvpMatrix );

Model* CreateModel();
void FreeModel( Model* model );
void SetModelTransformation( Model* model, glm::mat4 transformation );
void SetModelMesh( Model* model, Mesh* mesh );

#endif