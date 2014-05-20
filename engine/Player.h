#ifndef __PLAYER__
#define __PLAYER__

#include "Math.h"

bool InitPlayer();
void DestroyPlayer();
void DrawPlayer();
glm::mat4 GetPlayerViewMatrix();
void UpdateProjectionMatrix( int width, int height );
glm::mat4 GetPlayerProjectionMatrix();
void UpdatePlayer( float timeFrame );
glm::vec3 GetPlayerPosition();
glm::quat GetPlayerOrientation();

#endif
