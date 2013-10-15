#include "OpenGL.h"
#include "Shader.h"
#include "Texture.h"
#include "Model.h"
#include "Background.h"

GLuint  g_SkyboxShader;
GLuint  g_SkyboxTexture;
Model   g_SkyboxModel;

bool InitBackground()
{
    g_SkyboxShader = LoadShader("Shaders/Skybox.vert", "Shaders/Skybox.frag");
    if(!g_SkyboxShader)
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
    FreeShader(g_SkyboxShader);
}

void DrawBackground()
{
    glPushMatrix();
    //glScalef(25,25,25);
    glDepthMask(false);

    glRotatef((float) glfwGetTime() * 50.f, 0.f, 0.f, 1.f);
    glBegin(GL_TRIANGLES);
        glColor3f(1.f, 0.f, 0.f); glVertex3f(-0.6f, -0.4f, 0.f);
        glColor3f(0.f, 1.f, 0.f); glVertex3f( 0.6f, -0.4f, 0.f);
        glColor3f(0.f, 0.f, 1.f); glVertex3f(  0.f,  0.6f, 0.f);
    glEnd();

    /*
    BindShader(g_SkyboxShader);
    BindTexture(GL_TEXTURE_CUBE_MAP, g_SkyboxTexture, 0);
    */
    DrawModel(&g_SkyboxModel);

    glDepthMask(true);
    glPopMatrix();
}
