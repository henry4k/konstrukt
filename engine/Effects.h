#ifndef __EFFECTS__
#define __EFFECTS__

#include "Math.h"
#include "Shader.h"

bool InitEffects();
void DestroyEffects();
void SetModelViewProjectionMatrix( Program program, const glm::mat4* mvpMatrix );
Program GetDefaultProgram();
void BeginRenderShadowTexture();
void EndRenderShadowTexture();
void BeginRender();
void EndRender();
void SetLight( glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 direction );

#endif
