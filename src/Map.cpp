#include <vector>
#include <string.h>
#include <stdint.h>
#include <float.h>
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

Program g_MapProgram;
Texture g_MapTexture;
MeshBuffer g_MapMeshBuffer;
Model g_MapModel;

SolidBuffer g_MapStaticSolid;

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

bool GenerateStaticMapSolid( SolidBuffer* solid, int startX, int startZ, int endX, int endZ )
{
    solid->clear();

    for(int z = startZ; z < endZ; ++z)
    for(int x = startX; x < endX; ++x)
    {
        const Tile* tile = &g_MapTileData[z*g_MapWidth + x];
        const TileDefinition* tileDef = &g_TileDefinitions[tile->definition];

        if(tileDef->staticSolidFn)
        {
            tileDef->staticSolidFn(tile->definition, x, z, solid);
        }
    }

    Log("Created map chunk solid from %d,%d to %d,%d with %d Aabbs.", startX, startZ, endX, endZ, solid->size());
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
    BindProgram(g_MapProgram);
    BindTexture(GL_TEXTURE_2D, g_MapTexture, 0);
    DrawModel(&g_MapModel);
}

void UpdateMap( int startX, int startZ, int endX, int endZ )
{
    FreeModel(&g_MapModel);
    GenerateStaticMapModel(&g_MapModel, 0,0, g_MapWidth, g_MapDepth);

    GenerateStaticMapSolid(&g_MapStaticSolid, 0,0, g_MapWidth, g_MapDepth);
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

float RayTestMap( glm::vec3 rayOrigin, glm::vec3 rayDirection, float maxLength )
{
    const Ray ray(rayOrigin, rayDirection);
    float minLength = FLT_MAX;

    for(int i = 0; i < g_MapStaticSolid.size(); ++i)
    {
        const Aabb aabb = g_MapStaticSolid[i];

        float currentLength;
        if(RayTestAabb(ray, aabb, &currentLength) &&
           currentLength >= 0 &&
           minLength > currentLength)
        {
            minLength = currentLength;
        }
    }

    return minLength;
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

    for(int i = 0; i < g_MapStaticSolid.size(); ++i)
    {
        const Aabb aabb = g_MapStaticSolid[i];

        Box solidBox;
        solidBox.position  = aabb.position;
        solidBox.halfWidth = aabb.halfWidth;
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

    for(int i = 0; i < g_MapStaticSolid.size(); ++i)
    {
        const Aabb aabb = g_MapStaticSolid[i];

        Box solidBox;
        solidBox.position  = aabb.position;
        solidBox.halfWidth = aabb.halfWidth;
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

void OnSquirrelGenerateStaticTileSolid( int tileDefinition, int x, int z, SolidBuffer* buffer )
{
    HSQUIRRELVM vm = GetSquirrelVM();
    sq_pushinteger(vm, tileDefinition);
    sq_pushinteger(vm, x);
    sq_pushinteger(vm, z);
    if(FireSquirrelCallback(SQCALLBACK_STATIC_TILE_SOLID_GENRATOR, 3, true))
    {
        void* blob = NULL;
        sqstd_getblob(vm, -1, (SQUserPointer*)&blob);
        const int blobSize = sqstd_getblobsize(vm, -1);

        if(blobSize % sizeof(Aabb) != 0)
        {
            Error("GenerateStaticMapSolid: Blob size is not a multiple of the aabb size!");
            return;
        }

        const int aabbCount = blobSize / sizeof(Aabb);
        const Aabb* aabbList = reinterpret_cast<const Aabb*>(blob);

        for(int i = 0; i < aabbCount; ++i)
        {
            const Aabb aabb = aabbList[i];
            // A quick sanity check:
            assert(aabb.halfWidth.x > 0);
            assert(aabb.halfWidth.y > 0);
            assert(aabb.halfWidth.z > 0);
            buffer->push_back(aabbList[i]);
        }

        sq_pop(vm, 1); // Pop return value.
    }
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

SQInteger Squirrel_RayTestMap( HSQUIRRELVM vm )
{
    glm::vec3 rayOrigin;
    sq_getfloat(vm, 2, &rayOrigin.x);
    sq_getfloat(vm, 3, &rayOrigin.y);
    sq_getfloat(vm, 4, &rayOrigin.z);

    glm::vec3 rayDirection;
    sq_getfloat(vm, 5, &rayDirection.x);
    sq_getfloat(vm, 6, &rayDirection.y);
    sq_getfloat(vm, 7, &rayDirection.z);

    const float length = RayTestMap(rayOrigin, rayDirection, 42);
    sq_pushfloat(vm, length);
    return 1;
}
RegisterStaticFunctionInSquirrel(RayTestMap, 7, ".ffffff");

enum DataType
{
    DATA_TYPE_UINT8,
    DATA_TYPE_INT8,
    DATA_TYPE_UINT16,
    DATA_TYPE_INT16,
    DATA_TYPE_UINT32,
    DATA_TYPE_INT32,
    DATA_TYPE_FLOAT32,
    DATA_TYPE_COUNT
};

static const int DATA_TYPE_SIZE[DATA_TYPE_COUNT] =
{
    sizeof(int8_t),
    sizeof(uint8_t),
    sizeof(int16_t),
    sizeof(uint16_t),
    sizeof(int32_t),
    sizeof(uint32_t),
    sizeof(float)
};

SQInteger Squirrel_GetTileDataAt( HSQUIRRELVM vm )
{
    SQInteger x = 0;
    sq_getinteger(vm, 2, &x);

    SQInteger z = 0;
    sq_getinteger(vm, 3, &z);

    SQInteger position = 0;
    sq_getinteger(vm, 4, &position);

    SQInteger type = 0;
    sq_getinteger(vm, 5, &type);

    if(type < 0 || type >= DATA_TYPE_COUNT)
        return sq_throwerror(vm, "Invalid data type");

    if(position+DATA_TYPE_SIZE[type] >= TILE_DATA_SIZE)
        return sq_throwerror(vm, "Data segment out of bounds");

    const Tile* tile = GetTileAt(x,z);
    if(!tile)
        return sq_throwerror(vm, "Position out of range");

    const void* data = &tile->data[position];
    switch(type)
    {
#define GET_VALUE(D,T) sq_push##D(vm, *reinterpret_cast< const T *>(data))
        case DATA_TYPE_UINT8:   GET_VALUE(integer, uint8_t ); break;
        case DATA_TYPE_INT8:    GET_VALUE(integer, uint8_t ); break;
        case DATA_TYPE_UINT16:  GET_VALUE(integer, uint16_t); break;
        case DATA_TYPE_INT16:   GET_VALUE(integer, uint16_t); break;
        case DATA_TYPE_UINT32:  GET_VALUE(integer, uint32_t); break;
        case DATA_TYPE_INT32:   GET_VALUE(integer, uint32_t); break;
        case DATA_TYPE_FLOAT32: GET_VALUE(float, float); break;
#undef GET_VALUE
        default:
            FatalError("Unknown enum value!");
    }
    return 1;
}
RegisterStaticFunctionInSquirrel(GetTileDataAt, 5, ".iiii");

SQInteger Squirrel_SetTileDataAt( HSQUIRRELVM vm )
{
    SQInteger x = 0;
    sq_getinteger(vm, 2, &x);

    SQInteger z = 0;
    sq_getinteger(vm, 3, &z);

    SQInteger position = 0;
    sq_getinteger(vm, 4, &position);

    SQInteger type = 0;
    sq_getinteger(vm, 5, &type);

    if(type < 0 || type >= DATA_TYPE_COUNT)
        return sq_throwerror(vm, "Invalid data type");

    if(position+DATA_TYPE_SIZE[type] >= TILE_DATA_SIZE)
        return sq_throwerror(vm, "Data segment out of bounds");

    SQFloat floatValue = 0;
    SQInteger intValue = 0;
    if(type == DATA_TYPE_FLOAT32)
        sq_getfloat(vm, 6, &floatValue);
    else
        sq_getinteger(vm, 6, &intValue);

    Tile* tile = GetTileAt(x,z);
    if(!tile)
        return sq_throwerror(vm, "Position out of range");

    void* data = &tile->data[position];
    switch(type)
    {
#define SET_VALUE(T,O) *reinterpret_cast< T *>(data) = O
        case DATA_TYPE_UINT8:   SET_VALUE(uint8_t , intValue); break;
        case DATA_TYPE_INT8:    SET_VALUE(uint8_t , intValue); break;
        case DATA_TYPE_UINT16:  SET_VALUE(uint16_t, intValue); break;
        case DATA_TYPE_INT16:   SET_VALUE(uint16_t, intValue); break;
        case DATA_TYPE_UINT32:  SET_VALUE(uint32_t, intValue); break;
        case DATA_TYPE_INT32:   SET_VALUE(uint32_t, intValue); break;
        case DATA_TYPE_FLOAT32: SET_VALUE(float, floatValue); break;
#undef SET_VALUE
        default:
            FatalError("Unknown enum value!");
    }
    return 0;
}
RegisterStaticFunctionInSquirrel(SetTileDataAt, 6, ".iiiii|f");
