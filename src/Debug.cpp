#include <string.h>
#include <vector>
#include "Common.h"
#include "Config.h"
#include "OpenGL.h"
#include "Vertex.h"
#include "Shader.h"
#include "Debug.h"

bool g_DebugModes[DEBUG_MODE_COUNT];
std::vector<Vertex> g_DebugVertexBuffer;
glm::vec3 g_DebugLineColor;
Program g_DebugProgram;

bool InitDebug()
{
    memset(g_DebugModes, 0, sizeof(g_DebugModes));

#define GET_DEBUG_MODE(I,N) g_DebugModes[(I)] = GetConfigBool((N), false);
    GET_DEBUG_MODE(DEBUG_COLLISION, "debug.collision")
#undef GET_DEBUG_MODE

    g_DebugLineColor = glm::vec3(0,0,0);

    g_DebugProgram = LoadProgram("Shaders/Debug.vert", "Shaders/Debug.frag");
    if(!g_DebugProgram)
        return false;

    return true;
}

void DestroyDebug()
{
    g_DebugVertexBuffer.clear();
    FreeProgram(g_DebugProgram);
}

bool IsDebugging( DebugMode mode )
{
    return g_DebugModes[mode];
}

void AddDebugLine( glm::vec3 startPosition, glm::vec3 startColor, glm::vec3 endPosition, glm::vec3 endColor )
{
    Vertex vertex;
    memset(&vertex, 0, sizeof(Vertex));

    vertex.position = startPosition;
    vertex.color = startColor;
    g_DebugVertexBuffer.push_back(vertex);

    vertex.position = endPosition;
    vertex.color = endColor;
    g_DebugVertexBuffer.push_back(vertex);
}

void SetDebugLineColor( glm::vec3 color )
{
    g_DebugLineColor = color;
}

void AddDebugLine( glm::vec3 startPosition, glm::vec3 endPosition )
{
    AddDebugLine(startPosition, g_DebugLineColor, endPosition, g_DebugLineColor);
}

void AddDebugQuad( glm::vec3 min, glm::vec3 max )
{
    // TODO
    assert(false);
}

bool CreateDebugModel( Model* model )
{
    Mesh mesh;
    memset(&mesh, 0, sizeof(Mesh));
    mesh.vertices = &g_DebugVertexBuffer.front();
    mesh.vertexCount = g_DebugVertexBuffer.size();

    const bool success = CreateModel(model, &mesh);
    model->primitiveType = GL_LINES;
    return success;
}

void DrawDebugModel( Model* model )
{
    BindProgram(g_DebugProgram);
    DrawModel(model);
}

void DrawDebugMesh()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    SetVertexAttributePointers(&g_DebugVertexBuffer.front());
    glDrawArrays(GL_LINES, 0, g_DebugVertexBuffer.size());
}

void FlushDebugMesh()
{
    g_DebugVertexBuffer.clear();
}
