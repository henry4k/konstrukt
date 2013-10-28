#ifndef __DEBUG__
#define __DEBUG__

#include "Math.h"
#include "Model.h"

enum DebugMode
{
	DEBUG_OPENGL,
	DEBUG_LUA,
    DEBUG_COLLISION,
    //DEBUG_PRESSURE,
    //DEBUG_LIGHTING
    DEBUG_MODE_COUNT
};

bool InitDebug();
bool InitDebugGraphics();
void DestroyDebug();

bool IsDebugging( DebugMode mode );

// 3D Stuff
void AddDebugLine( glm::vec3 startPosition, glm::vec3 startColor, glm::vec3 endPosition, glm::vec3 endColor );

void SetDebugLineColor( glm::vec3 color );
void AddDebugLine( glm::vec3 startPosition, glm::vec3 endPosition );
void AddDebugCube( glm::vec3 min, glm::vec3 max );
// ...

bool CreateDebugModel( Model* model );
void DrawDebugModel( Model* model );
void DrawDebugMesh();
void FlushDebugMesh();

#endif
