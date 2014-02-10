#include "Common.h"
#include "OpenGL.h"
#include "Shader.h"
#include "Texture.h"
#include "Model.h"
#include "Effects.h"
#include "Player.h"
#include "Background.h"

#include <glm/gtc/matrix_transform.hpp>

Program g_SkyboxProgram;
Texture g_SkyboxTexture;
Model   g_SkyboxModel;

bool InitBackground()
{
    g_SkyboxProgram = LoadProgram("Shaders/Skybox.vert", "Shaders/Skybox.frag");
    if(!g_SkyboxProgram)
        return false;

    g_SkyboxTexture = LoadCubeTexture(TEX_MIPMAP|TEX_FILTER, "Textures/DeepSpace1/%s.png");
    if(!g_SkyboxTexture)
        return false;

    if(!LoadModel(&g_SkyboxModel, "Meshes/Skybox.ply"))
        return false;

    return true;
}

void DestroyBackground()
{
    FreeModel(&g_SkyboxModel);
    FreeTexture(g_SkyboxTexture);
    FreeProgram(g_SkyboxProgram);
}

void DrawBackground()
{
    static float rotation = 0;
    rotation += 0.00002f;

    //glDepthFunc(GL_ALWAYS);
    //glDepthMask(GL_FALSE);

    glm::mat4 modelMatrix;
    modelMatrix = glm::scale(modelMatrix, glm::vec3(100, 100, 100));
    modelMatrix = glm::rotate(modelMatrix, rotation, glm::vec3(0.1, 0.05, 0.025));

    const glm::mat4 modelViewProjectionMatrix =
        GetPlayerProjectionMatrix() *
        MakeRotationMatrix(GetPlayerViewMatrix()) *
        modelMatrix;

    BindProgram(g_SkyboxProgram);
    SetModelViewProjectionMatrix(g_SkyboxProgram, modelViewProjectionMatrix);
    BindTexture(GL_TEXTURE_CUBE_MAP, g_SkyboxTexture, 0);
    DrawModel(&g_SkyboxModel);

    //glDepthMask(GL_TRUE);
    //glDepthFunc(GL_LESS);
}
