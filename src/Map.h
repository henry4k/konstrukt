#ifndef __MAP__
#define __MAP__

#include "Math.h"
#include "Mesh.h"

static const int INVALID_TILE_DEFINITION = -1;

bool InitMap();
void DestroyMap();

typedef void (*StaticTileMeshFn)( int TileDefinition, int x, int z );
typedef void (*StaticTileSolidFn)( int TileDefinition, int x, int z );
int CreateTileDefinition( const char* name, StaticTileMeshFn staticMeshFn, StaticTileSolidFn staticSolidFn );
void AddMeshToTile( const Mesh* mesh, glm::vec3 translate, glm::vec3 rotate, glm::vec3 scale );
void AddSolidToTile( glm::vec3 min, glm::vec3 max );

void GenerateMap( int width, int depth );
void DrawMap();
void UpdateMap( int startX, int startZ, int endX, int endZ );

int GetTileDefinitionAt( int x, int z );
void SetTileAt( int x, int z, int definition );
bool CollidesWithMap( glm::vec3* out, glm::vec3 position, glm::vec3 halfWidth );

#endif
