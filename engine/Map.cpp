#include <vector>
#include <string.h>
#include <stdint.h>
#include <float.h>
#include "Common.h"
#include "Texture.h"
#include "Shader.h"
#include "MeshBuffer.h"
#include "Mesh.h"
#include "Game.h"
#include "Debug.h"
#include "Map.h"

#include <glm/gtc/matrix_transform.hpp>

static const int MAX_TILE_NAME_LENGTH = 31;
static const int MAX_TILE_CHARS = 7;
const glm::vec3 TILE_SIZE(0.5, 2.0, 0.5);

struct TileDefinition
{
    char name[MAX_TILE_NAME_LENGTH+1];
    StaticTileMeshFn staticMeshFn;
    StaticTileSolidFn staticSolidFn;
};


const int TILE_DATA_SIZE = 6;
struct Tile
{
    uint16_t definition;
    uint8_t data[TILE_DATA_SIZE];
}; // 16 + 8*6 = 64 (8 Byte)

std::vector<TileDefinition> g_TileDefinitions;
int g_MapWidth = 0;
int g_MapDepth = 0;
Tile* g_MapTileData = NULL;

Texture g_MapTexture;
MeshBuffer g_MapMeshBuffer;
Mesh g_MapMesh;

SolidBuffer g_MapSolidBuffer;

bool InitMap()
{
    g_TileDefinitions.clear();
    g_MapTileData = NULL;


    g_MapTexture = Load2dTexture(TEX_CLAMP, "Textures/Tiles.png");
    if(!g_MapTexture)
        return false;

    CreateMeshBuffer(&g_MapMeshBuffer);
    memset(&g_MapMesh, 0, sizeof(Mesh));

    return true;
}

void DestroyMap()
{
    FreeTexture(g_MapTexture);
    FreeMesh(&g_MapMesh);
    FreeMeshBuffer(&g_MapMeshBuffer);
}

/**
 * @return
 *      The index of the definition or `INVALID_TILE_DEFINITION` if it failed.
 */
int CreateTileDefinition( const char* name, StaticTileMeshFn staticMeshFn, StaticTileSolidFn staticSolidFn )
{
    TileDefinition definition;

    strncpy(definition.name, name, MAX_TILE_NAME_LENGTH);
    definition.name[MAX_TILE_NAME_LENGTH] = '\0';

    definition.staticMeshFn = staticMeshFn;
    definition.staticSolidFn = staticSolidFn;

    g_TileDefinitions.push_back(definition);
    return g_TileDefinitions.size()-1;
}

bool GenerateStaticMapMesh( Mesh* target, int startX, int startZ, int endX, int endZ )
{
    FreeMeshBuffer(&g_MapMeshBuffer);
    CreateMeshBuffer(&g_MapMeshBuffer);

    for(int z = startZ; z < endZ; ++z)
    for(int x = startX; x < endX; ++x)
    {
        const Tile* tile = &g_MapTileData[z*g_MapWidth + x];
        const TileDefinition* tileDef = &g_TileDefinitions[tile->definition];

        if(tileDef->staticMeshFn)
        {
            tileDef->staticMeshFn(tile->definition, x, z, &g_MapMeshBuffer);
        }
    }

    Log("Created map chunk mesh from %d,%d to %d,%d with %d vertices and %d indices.", startX, startZ, endX, endZ, g_MapMeshBuffer.vertices.size(), g_MapMeshBuffer.indices.size());

    return CreateMesh(target, &g_MapMeshBuffer);
}

bool GenerateStaticMapSolid( SolidBuffer* solidBuffer, int startX, int startZ, int endX, int endZ )
{
    solidBuffer->clear();

    for(int z = startZ; z < endZ; ++z)
    for(int x = startX; x < endX; ++x)
    {
        const Tile* tile = &g_MapTileData[z*g_MapWidth + x];
        const TileDefinition* tileDef = &g_TileDefinitions[tile->definition];

        if(tileDef->staticSolidFn)
        {
            tileDef->staticSolidFn(tile->definition, x, z, solidBuffer);
        }
    }

    Log("Created map chunk solid buffer from %d,%d to %d,%d with %d solids.", startX, startZ, endX, endZ, solidBuffer->size());
    return true;
}

void GenerateMap( int width, int depth )
{
    if(g_MapTileData)
        delete[] g_MapTileData;

    g_MapWidth = width;
    g_MapDepth = depth;
    g_MapTileData = new Tile[g_MapWidth*g_MapDepth];
    memset(g_MapTileData, 0, sizeof(Tile)*g_MapWidth*g_MapDepth);

    //UpdateMap(0, 0, g_MapWidth, g_MapDepth);
}

void DrawMap()
{
    BindTexture(GL_TEXTURE_2D, g_MapTexture, 0);
    DrawMesh(&g_MapMesh);
}

void UpdateMap( int startX, int startZ, int endX, int endZ )
{
    FreeMesh(&g_MapMesh);
    GenerateStaticMapMesh(&g_MapMesh, 0,0, g_MapWidth, g_MapDepth);

    GenerateStaticMapSolid(&g_MapSolidBuffer, 0,0, g_MapWidth, g_MapDepth);
}

Tile* GetTileAt( int x, int z )
{
    if(x < 0 || x >= g_MapWidth)
        return NULL;

    if(z < 0 || z >= g_MapDepth)
        return NULL;

    if(g_MapTileData == NULL)
        return NULL;

    return &g_MapTileData[z*g_MapWidth + x];
}

int GetTileDefinitionAt( int x, int z )
{
    const Tile* tile = GetTileAt(x,z);
    return tile ? tile->definition : INVALID_TILE_DEFINITION;
}

void SetTileAt( int x, int z, int definition )
{
    Tile* tile = GetTileAt(x,z);
    if(tile)
    {
        tile->definition = definition;
    }
}

struct MapRayTestResult
{
    float length;
    int tileX;
    int tileZ;
    int solidUserData;
};

MapRayTestResult RayTestMap( glm::vec3 rayOrigin, glm::vec3 rayDirection, float maxLength )
{
    const Ray ray(rayOrigin, rayDirection);
    float minLength = FLT_MAX;
    int solidIndex = -1;

    for(int i = 0; i < g_MapSolidBuffer.size(); ++i)
    {
        const Solid solid = g_MapSolidBuffer[i];

        float currentLength;
        if(RayTestAabb(ray, solid.aabb, &currentLength) &&
           currentLength >= 0 &&
           minLength > currentLength)
        {
            minLength = currentLength;
            solidIndex = i;
        }
    }

    MapRayTestResult result;
    result.length = minLength;
    if(solidIndex >= 0)
    {
        const Solid solid = g_MapSolidBuffer[solidIndex];
        result.tileX = solid.tileX;
        result.tileZ = solid.tileZ;
        result.solidUserData = solid.userData;
    }
    return result;
}

void DrawBoxCollisionInMap( const Box* box )
{
    using namespace glm;

    FlushDebugMesh();

    SetDebugLineColor(vec3(1,1,0));
    AddDebugCube(
        box->position-box->halfWidth,
        box->position+box->halfWidth
    );

    for(int i = 0; i < g_MapSolidBuffer.size(); ++i)
    {
        const Solid solid = g_MapSolidBuffer[i];

        Box solidBox;
        solidBox.position  = solid.aabb.position;
        solidBox.halfWidth = solid.aabb.halfWidth;
        solidBox.velocity  = vec3(0, 0, 0);

        if(TestAabbOverlap(*box, solidBox))
            SetDebugLineColor(vec3(1,0,0));
        else
            SetDebugLineColor(vec3(0,1,0));

        AddDebugCube(
            solidBox.position-solidBox.halfWidth,
            solidBox.position+solidBox.halfWidth
        );
    }

    DrawDebugMesh();
}

void SimulateBoxInMap( Box* box, float timeFrame )
{
    using namespace glm;

    const float BOUNCE = 1.0f;
    const float SLIDE  = 1.0f;

    for(int i = 0; i < g_MapSolidBuffer.size(); ++i)
    {
        const Solid solid = g_MapSolidBuffer[i];

        Box solidBox;
        solidBox.position  = solid.aabb.position;
        solidBox.halfWidth = solid.aabb.halfWidth;
        solidBox.velocity  = vec3(0, 0, 0);

        vec3 penetration;
        if(TestAabbOverlap(*box, solidBox, &penetration))
        {
            box->position += penetration;
        }
        else
        {
            vec3 normal;
            const float collisionTime = SweptAabb(*box, solidBox, &normal, timeFrame);
            //Log("collisionTime = %.2f", collisionTime);

            box->position += box->velocity * collisionTime;

            for(int i = 0; i < 3; ++i)
                if(abs(normal[i]) > 0.0001f)
                    box->velocity[i] *= -1;

            //velocityChange += SLIDE * proj(a.velocity, normal);

            const float remainingTime = timeFrame-collisionTime;
            timeFrame = remainingTime;
        }
    }

    box->position += box->velocity * timeFrame;
}
