#ifndef __EFFECTS__
#define __EFFECTS__

#include "Math.h"

bool InitEffects();
void DestroyEffects();

void BindDefaultProgram();

void SetLight( glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 direction );

#endif
