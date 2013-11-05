#include <vector>
#include <string.h>
#include "Common.h"
#include "Texture.h"
#include "Shader.h"
#include "Mesh.h"
#include "Model.h"
#include "Debug.h"
#include "Map.h"

#include <glm/gtc/matrix_transform.hpp>

static const int MAX_TILE_NAME_LENGTH = 31;
static const int MAX_TILE_CHARS = 7;

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

Program g_MapProgram;
Texture g_MapTexture;
std::vector<TileDefinition> g_TileDefinitions;
int g_MapWidth = 0;
int g_MapDepth = 0;
Tile* g_MapTileData = NULL;

Model g_MapModel;

bool InitMap()
{
    g_MapProgram = LoadProgram("Shaders/Default.vert", "Shaders/Default.frag");
    if(!g_MapProgram)
        return false;

    g_MapTexture = Load2dTexture(0, "Textures/Tiles.png");
    if(!g_MapTexture)
        return false;

    g_TileDefinitions.clear();
    memset(&g_MapModel, 0, sizeof(Model));

    g_MapTileData = NULL;

    return true;
}

void DestroyMap()
{
    FreeProgram(g_MapProgram);
    FreeTexture(g_MapTexture);
    FreeModel(&g_MapModel);
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

std::vector<Vertex> g_MapVertexBuffer;
std::vector<unsigned short> g_MapIndexBuffer;

void AddMeshToMapBuffer( const Mesh* source, const glm::mat4* transform )
{
    // Allocate memory
    g_MapVertexBuffer.reserve(g_MapVertexBuffer.size()+source->vertexCount);
    g_MapIndexBuffer.reserve(g_MapIndexBuffer.size()+source->indexCount);

    const unsigned short indexOffset = g_MapVertexBuffer.size();

    for(int i = 0; i < source->vertexCount; ++i)
    {
        Vertex vertex = source->vertices[i];
        const glm::vec4 transformedPosition = *transform * glm::vec4(vertex.position, 1);
        vertex.position.x = transformedPosition.x;
        vertex.position.y = transformedPosition.y;
        vertex.position.z = transformedPosition.z;

        g_MapVertexBuffer.push_back(vertex);
    }

    for(int i = 0; i < source->indexCount; ++i)
    {
        g_MapIndexBuffer.push_back( indexOffset + source->indices[i] );
    }
}

glm::vec3 g_MapBufferOffset;
static const glm::mat4 MAT4_IDENTITY;

void AddMeshToTile( const Mesh* mesh, glm::vec3 translate, glm::vec3 rotate, glm::vec3 scale )
{
    glm::mat4 transform = glm::translate(MAT4_IDENTITY, g_MapBufferOffset+translate);
    transform = glm::rotate(transform, rotate.x, glm::vec3(1,0,0));
    transform = glm::rotate(transform, rotate.y, glm::vec3(0,1,0));
    transform = glm::rotate(transform, rotate.z, glm::vec3(0,0,1));
    transform = glm::scale(transform, scale);

    AddMeshToMapBuffer(mesh, &transform);
}

bool GenerateStaticMapModel( Model* target, int startX, int startZ, int endX, int endZ )
{
    g_MapVertexBuffer.clear();
    g_MapIndexBuffer.clear();

    for(int z = startZ; z < endZ; ++z)
    for(int x = startX; x < endX; ++x)
    {
        const Tile* tile = &g_MapTileData[z*g_MapWidth + x];
        const TileDefinition* tileDef = &g_TileDefinitions[tile->definition];

        if(tileDef->staticMeshFn)
        {
            g_MapBufferOffset = glm::vec3(x,0,z);
            tileDef->staticMeshFn(tile->definition, x, z);
        }
    }

    const int vertexCount = g_MapVertexBuffer.size();
    const int indexCount = g_MapIndexBuffer.size();

    Mesh mesh;
    mesh.vertexCount = vertexCount;
    mesh.vertices = &g_MapVertexBuffer.front();
    mesh.indexCount = indexCount;
    mesh.indices = &g_MapIndexBuffer.front();

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

    const int minX = max(0.0f, floor(box->position.x - box->halfWidth.x)-1.0f);
    const int minZ = max(0.0f, floor(box->position.z - box->halfWidth.z)-1.0f);

    const int maxX = min(float(g_MapWidth), floor(box->position.x + box->halfWidth.x)+1.0f);
    const int maxZ = min(float(g_MapDepth), floor(box->position.z + box->halfWidth.z)+1.0f);

    for(int z = minZ; z <= maxZ; ++z)
    for(int x = minX; x <= maxX; ++x)
    {
        if(GetTileDefinitionAt(x,z) != 0)
        {
            Box tileBox;
            tileBox.position  = vec3(x, 1, z);
            tileBox.halfWidth = vec3(0.5, 0.5, 0.5);
            tileBox.velocity  = vec3(0, 0, 0);

            vec3 collisionNormal;
            const float collisionTime = SweptAABB(*box, tileBox, &collisionNormal, 1);

            if(collisionTime < 1.0f)
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

    const int minX = max(0.0f, floor(box->position.x - box->halfWidth.x)-1.0f);
    const int minZ = max(0.0f, floor(box->position.z - box->halfWidth.z)-1.0f);

    const int maxX = min(float(g_MapWidth), floor(box->position.x + box->halfWidth.x)+1.0f);
    const int maxZ = min(float(g_MapDepth), floor(box->position.z + box->halfWidth.z)+1.0f);

    for(int z = minZ; z <= maxZ; ++z)
    for(int x = minX; x <= maxX; ++x)
    {
        if(GetTileDefinitionAt(x,z) != 0)
        {
            Box tileBox;
            tileBox.position  = vec3(x, 1, z);
            tileBox.halfWidth = vec3(0.5, 0.5, 0.5);
            tileBox.velocity  = vec3(0, 0, 0);

            vec3 collisionNormal;
            const float collisionTime = SweptAABB(*box, tileBox, &collisionNormal, timeFrame);
			float remainingTime = timeFrame-collisionTime;
			Log("collisionTime = %.2f remainingTime = %.2f", collisionTime, remainingTime);

			vec3 velocityChange(0,0,0);

			for(int i = 0; i < 3; ++i)
				if(abs(normal[i]) > 0.0001f)
					velocityChange[i] += BOUNCE * -a.velocity[i]*2;

			//velocityChange += SLIDE * proj(a.velocity, normal);

			Log("velocityChange = %.2f|%.2f|%.2f", velocityChange.x, velocityChange.y, velocityChange.z);

			box->velocity += velocityChange;

            box->position += box->velocity * collisionTime;
        }
    }
}
