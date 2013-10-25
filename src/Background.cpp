#include "OpenGL.h"
#include "Shader.h"
#include "Texture.h"
#include "Model.h"
#include "Background.h"

Program g_SkyboxProgram;
Texture g_SkyboxTexture;
Model   g_SkyboxModel;

bool InitBackground()
{
    g_SkyboxProgram = LoadProgram("Shaders/Skybox.vert", "Shaders/Skybox.frag");
    if(!g_SkyboxProgram)
        return false;

    g_SkyboxTexture = LoadCubeTexture(TEX_MIPMAP|TEX_FILTER, "Textures/Debug/%s.png");
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
    glPushMatrix();
    glScalef(100,100,100);
    //glDepthFunc(GL_ALWAYS);
    //glDepthMask(GL_FALSE);

    BindProgram(g_SkyboxProgram);
    BindTexture(GL_TEXTURE_CUBE_MAP, g_SkyboxTexture, 0);
    DrawModel(&g_SkyboxModel);

    //glDepthMask(GL_TRUE);
    //glDepthFunc(GL_LESS);
    glPopMatrix();
}
