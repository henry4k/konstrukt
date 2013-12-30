#include <vector>
#include <string.h>
#include "Common.h"
#include "Texture.h"
#include "Shader.h"
#include "Mesh.h"
#include "Model.h"
#include "Game.h"
#include "Debug.h"
#include "Squirrel.h"
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

struct Tile
{
    unsigned short definition;
};

std::vector<TileDefinition> g_TileDefinitions;
int g_MapWidth = 0;
int g_MapDepth = 0;
Tile* g_MapTileData = NULL;

Program g_MapProgram;
Texture g_MapTexture;
MeshBuffer g_MapMeshBuffer;
Model g_MapModel;

bool InitMap()
{
    g_TileDefinitions.clear();
    g_MapTileData = NULL;


    g_MapProgram = LoadProgram("Shaders/Default.vert", "Shaders/Default.frag");
    if(!g_MapProgram)
        return false;

    g_MapTexture = Load2dTexture(0, "Textures/Tiles.png");
    if(!g_MapTexture)
        return false;

    CreateMeshBuffer(&g_MapMeshBuffer);
    memset(&g_MapModel, 0, sizeof(Model));

    return true;
}

void DestroyMap()
{
    FreeProgram(g_MapProgram);
    FreeTexture(g_MapTexture);
    FreeModel(&g_MapModel);
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

bool GenerateStaticMapModel( Model* target, int startX, int startZ, int endX, int endZ )
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

    Mesh mesh;
    BuildMesh(&g_MapMeshBuffer, &mesh);
    Log("Created map chunk mesh from %d,%d to %d,%d with %d vertices and %d indices.", startX, startZ, endX, endZ, mesh.vertexCount, mesh.indexCount);

    return CreateModel(target, &mesh);
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
    BindProgram(g_MapProgram);
    BindTexture(GL_TEXTURE_2D, g_MapTexture, 0);
    DrawModel(&g_MapModel);
}

void UpdateMap( int startX, int startZ, int endX, int endZ )
{
    FreeModel(&g_MapModel);
    GenerateStaticMapModel(&g_MapModel, 0,0, g_MapWidth, g_MapDepth);
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

void DrawBoxCollisionInMap( const Box* box )
{
    using namespace glm;

    FlushDebugMesh();

    SetDebugLineColor(vec3(1,1,0));
    AddDebugCube(
        box->position-box->halfWidth,
        box->position+box->halfWidth
    );

    const int minX = max(0.0f, floor((box->position.x - box->halfWidth.x)/TILE_SIZE.x));
    const int minZ = max(0.0f, floor((box->position.z - box->halfWidth.z)/TILE_SIZE.z));

    const int maxX = min(ceil((box->position.x + box->halfWidth.x)/TILE_SIZE.x), float(g_MapWidth-1));
    const int maxZ = min(ceil((box->position.z + box->halfWidth.z)/TILE_SIZE.z), float(g_MapDepth-1));

    /*
    Log("min: %.2f,%.2f => %d,%d  max: %.2f,%.2f => %d,%d",
        box->position.x - box->halfWidth.x,
        box->position.z - box->halfWidth.z,
        minX,
        minZ,
        box->position.x + box->halfWidth.x,
        box->position.z + box->halfWidth.z,
        maxX,
        maxZ
    );
    return;
    */

    for(int z = minZ; z <= maxZ; ++z)
    for(int x = minX; x <= maxX; ++x)
    {
        if(GetTileDefinitionAt(x,z) != 0)
        {
            Box tileBox;
            tileBox.position  = vec3(x, 0.5f, z) * TILE_SIZE;
            tileBox.halfWidth = TILE_SIZE*0.5f;
            tileBox.velocity  = vec3(0, 0, 0);

            if(TestAABBOverlap(*box, tileBox))
                SetDebugLineColor(vec3(1,0,0));
            else
                SetDebugLineColor(vec3(0,1,0));

            AddDebugCube(
                tileBox.position-tileBox.halfWidth,
                tileBox.position+tileBox.halfWidth
            );
        }
    }

    DrawDebugMesh();
}

void SimulateBoxInMap( Box* box, float timeFrame )
{
    using namespace glm;

    const float BOUNCE = 1.0f;
    const float SLIDE  = 1.0f;

    const int minX = max(0.0f, floor((box->position.x - box->halfWidth.x)/TILE_SIZE.x));
    const int minZ = max(0.0f, floor((box->position.z - box->halfWidth.z)/TILE_SIZE.z));

    const int maxX = min(ceil((box->position.x + box->halfWidth.x)/TILE_SIZE.x), float(g_MapWidth-1));
    const int maxZ = min(ceil((box->position.z + box->halfWidth.z)/TILE_SIZE.z), float(g_MapDepth-1));

    for(int z = minZ; z <= maxZ; ++z)
    for(int x = minX; x <= maxX; ++x)
    {
        if(GetTileDefinitionAt(x,z) != 0)
        {
            Box tileBox;
            tileBox.position  = vec3(x, 0.5f, z) * TILE_SIZE;
            tileBox.halfWidth = TILE_SIZE*0.5f;
            tileBox.velocity  = vec3(0, 0, 0);

            vec3 penetration;
            if(TestAABBOverlap(*box, tileBox, &penetration))
            {
                box->position += penetration;
            }
            else
            {
                vec3 normal;
                const float collisionTime = SweptAABB(*box, tileBox, &normal, timeFrame);
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
    }

    box->position += box->velocity * timeFrame;
}


// --- Squirrel Bindings ---

void OnSquirrelGenerateStaticTileMesh( int tileDefinition, int x, int z, MeshBuffer* buffer )
{
    HSQUIRRELVM vm = GetSquirrelVM();
    sq_pushinteger(vm, tileDefinition);
    sq_pushinteger(vm, x);
    sq_pushinteger(vm, z);
    PushUserDataToSquirrel(vm, &buffer, sizeof(MeshBuffer*), NULL);
    FireSquirrelCallback(SQCALLBACK_STATIC_TILE_MESH_GENRATOR, 4, false);
}

SQInteger Squirrel_RegisterStaticTileMeshGeneratorCallback( HSQUIRRELVM vm )
{
    SetSquirrelCallback(SQCALLBACK_STATIC_TILE_MESH_GENRATOR, vm, 2);
    return 0;
}
RegisterStaticFunctionInSquirrel(RegisterStaticTileMeshGeneratorCallback, 2, ".c");

void OnSquirrelGenerateStaticTileSolid( int TileDefinition, int x, int z )
{
}

SQInteger Squirrel_RegisterStaticTileSolidGeneratorCallback( HSQUIRRELVM vm )
{
    SetSquirrelCallback(SQCALLBACK_STATIC_TILE_SOLID_GENRATOR, vm, 2);
    return 0;
}
RegisterStaticFunctionInSquirrel(RegisterStaticTileSolidGeneratorCallback, 2, ".c");

SQInteger Squirrel_CreateTileDefinition( HSQUIRRELVM vm )
{
    const char* name = NULL;
    sq_getstring(vm, 2, &name);

    const int definitionId = CreateTileDefinition(
        name,
        OnSquirrelGenerateStaticTileMesh,
        OnSquirrelGenerateStaticTileSolid
    );

    sq_pushinteger(vm, definitionId);
    return 1;
}
RegisterStaticFunctionInSquirrel(CreateTileDefinition, 2, ".s");

SQInteger Squirrel_GenerateMap( HSQUIRRELVM vm )
{
    SQInteger width = 0;
    sq_getinteger(vm, 2, &width);

    SQInteger depth = 0;
    sq_getinteger(vm, 3, &depth);

    GenerateMap(width, depth);
    return 0;
}
RegisterStaticFunctionInSquirrel(GenerateMap, 3, ".ii");

SQInteger Squirrel_UpdateMap( HSQUIRRELVM vm )
{
    UpdateMap(0,0,0,0);
    return 0;
}
RegisterStaticFunctionInSquirrel(UpdateMap, 1, ".");

SQInteger Squirrel_GetTileDefinitionAt( HSQUIRRELVM vm )
{
    SQInteger x = 0;
    sq_getinteger(vm, 2, &x);

    SQInteger z = 0;
    sq_getinteger(vm, 3, &z);

    sq_pushinteger(vm, GetTileDefinitionAt(x,z));
    return 1;
}
RegisterStaticFunctionInSquirrel(GetTileDefinitionAt, 3, ".ii");

SQInteger Squirrel_SetTileAt( HSQUIRRELVM vm )
{
    SQInteger x = 0;
    sq_getinteger(vm, 2, &x);

    SQInteger z = 0;
    sq_getinteger(vm, 3, &z);

    SQInteger definitionId = 0;
    sq_getinteger(vm, 4, &definitionId);

    SetTileAt(x,z,definitionId);
    return 0;
}
RegisterStaticFunctionInSquirrel(SetTileAt, 4, ".iii");
