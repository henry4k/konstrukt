#include "Common.h"
#include "Map.h"
#include "Mesh.h"
#include "Test.h"

using namespace glm;

static const int MAP_WIDTH = 10;
static const int MAP_DEPTH = 10;

static const int MAP[] =
{
    0,0,0,0,1,0,1,0,0,0,
    0,0,0,0,1,0,1,0,0,0,
    1,1,1,1,1,0,1,1,1,0,
    0,0,0,0,0,0,0,0,1,0,
    1,1,1,0,1,1,1,0,1,1,
    1,0,0,0,0,1,1,0,0,0,
    1,0,0,0,0,1,1,1,1,1,
    1,0,0,0,0,1,0,0,0,0,
    1,0,0,0,0,1,0,0,0,0,
    1,1,1,1,1,1,0,0,0,0
};

int g_Void;

Mesh g_WallMesh;
int g_Wall;

void OnGenerateStaticTileMesh( int tileDefinition, int x, int y )
{
    AddMeshToTile(&g_WallMesh, vec3(0,0,0), vec3(0,0,0), vec3(1,1,1));
}

void OnGenerateStaticTileSolids( int TileDefinition, int x, int y )
{
    Log("OnGenerateStaticTileSolids");
    //AddSolidToTile(vec3(0,0,0), vec3(1,2,1));
}

bool InitTest()
{
    g_Void = CreateTileDefinition("Void", NULL, NULL);

    LoadMesh(&g_WallMesh, "Meshes/Block.ply");
    g_Wall = CreateTileDefinition("Wall", OnGenerateStaticTileMesh, OnGenerateStaticTileSolids);

    GenerateMap(MAP_WIDTH, MAP_DEPTH);

    for(int z = 0; z < MAP_DEPTH; ++z)
    for(int x = 0; x < MAP_WIDTH; ++x)
    {
        SetTileAt(x,z, MAP[z*MAP_WIDTH + x]);
    }

    UpdateMap(0,0,MAP_WIDTH-1,MAP_DEPTH-1);
    return true;
}

void DestroyTest()
{
    FreeMesh(&g_WallMesh);
}
