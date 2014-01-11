#ifndef __MAP__
#define __MAP__

#include <vector>
#include "Math.h"
#include "MeshBuffer.h"

typedef std::vector<Aabb> SolidBuffer;

static const int INVALID_TILE_DEFINITION = -1;

bool InitMap();
void DestroyMap();

typedef void (*StaticTileMeshFn)( int TileDefinition, int x, int z, MeshBuffer* buffer );
typedef void (*StaticTileSolidFn)( int TileDefinition, int x, int z, SolidBuffer* buffer );
int CreateTileDefinition( const char* name, StaticTileMeshFn staticMeshFn, StaticTileSolidFn staticSolidFn );
void AddSolidToTile( glm::vec3 min, glm::vec3 max );

void GenerateMap( int width, int depth );
void DrawMap();

int GetTileDefinitionAt( int x, int z );
void SetTileAt( int x, int z, int definition );
void DrawBoxCollisionInMap( const Box* box );
void SimulateBoxInMap( Box* box, float timeFrame );

#endif
