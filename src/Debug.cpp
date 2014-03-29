#include <string.h>
#include <vector>
#include "Common.h"
#include "Config.h"
#include "OpenGL.h"
#include "Vertex.h"
#include "Shader.h"
#include "Player.h"
#include "Effects.h"
#include "Debug.h"

static const char* DEBUG_MODE_NAMES[DEBUG_MODE_COUNT] =
{
    "debug.opengl",
    "debug.squirrel",
    "debug.collision"
};

bool g_DebugModes[DEBUG_MODE_COUNT];

std::vector<Vertex> g_DebugVertexBuffer;
glm::vec3 g_DebugLineColor;
Program g_DebugProgram;

bool InitDebug()
{
    memset(g_DebugModes, 0, sizeof(g_DebugModes));

    for(int i = 0; i < DEBUG_MODE_COUNT; ++i)
    {
        g_DebugModes[i] = GetConfigBool(DEBUG_MODE_NAMES[i], false);
    }

    return true;
}

bool InitDebugGraphics()
{
    g_DebugLineColor = glm::vec3(0,0,0);

    g_DebugProgram = LoadProgram("Shaders/Debug.vert", "Shaders/Debug.frag");
    if(!g_DebugProgram)
        return false;

    glLineWidth(3);
    //glEnable(GL_LINE_SMOOTH);

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

void AddDebugCube( glm::vec3 min, glm::vec3 max )
{
    using namespace glm;

    // Bottom quad
    AddDebugLine(vec3(min.x, min.y, min.z), vec3(max.x, min.y, min.z));
    AddDebugLine(vec3(max.x, min.y, min.z), vec3(max.x, min.y, max.z));
    AddDebugLine(vec3(max.x, min.y, max.z), vec3(min.x, min.y, max.z));
    AddDebugLine(vec3(min.x, min.y, max.z), vec3(min.x, min.y, min.z));

    // Top quad
    AddDebugLine(vec3(min.x, max.y, min.z), vec3(max.x, max.y, min.z));
    AddDebugLine(vec3(max.x, max.y, min.z), vec3(max.x, max.y, max.z));
    AddDebugLine(vec3(max.x, max.y, max.z), vec3(min.x, max.y, max.z));
    AddDebugLine(vec3(min.x, max.y, max.z), vec3(min.x, max.y, min.z));

    // Vertical stuff
    AddDebugLine(vec3(min.x, min.y, min.z), vec3(min.x, max.y, min.z));
    AddDebugLine(vec3(max.x, min.y, min.z), vec3(max.x, max.y, min.z));
    AddDebugLine(vec3(max.x, min.y, max.z), vec3(max.x, max.y, max.z));
    AddDebugLine(vec3(min.x, min.y, max.z), vec3(min.x, max.y, max.z));
}

void BeginDebugDrawing()
{
    const glm::mat4 modelViewProjectionMatrix =
        GetPlayerProjectionMatrix() *
        GetPlayerViewMatrix();

    BindProgram(g_DebugProgram);
    SetModelViewProjectionMatrix(g_DebugProgram, &modelViewProjectionMatrix);
}

void DrawDebugMesh()
{
    BeginDebugDrawing();
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    SetVertexAttributePointers(&g_DebugVertexBuffer.front());
    glDrawArrays(GL_LINES, 0, g_DebugVertexBuffer.size());
}

void FlushDebugMesh()
{
    g_DebugVertexBuffer.clear();
}
