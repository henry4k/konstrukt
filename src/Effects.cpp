#include "Common.h"
#include "Config.h"
#include "OpenGL.h"
#include "Texture.h"
#include "Window.h"
#include "Effects.h"

#include <glm/gtc/matrix_transform.hpp>

const glm::mat4 BIAS_MATRIX(
    0.5f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.5f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.5f, 0.0f,
    0.5f, 0.5f, 0.5f, 1.0f
);

Program g_DefaultProgram = 0;

int g_DepthTextureSize = 0; // See InitEffects
Texture g_DepthTexture = 0;

GLuint  g_Framebuffer = 0;
Program g_DepthProgram = 0;
glm::mat4 g_DepthMVP;

glm::vec3 g_AmbientLight;
glm::vec3 g_DiffuseLight;
glm::vec3 g_LightDirection;

bool InitEffects()
{
    g_DefaultProgram = LoadProgram("Shaders/Default.vert", "Shaders/Default.frag");
    if(!g_DefaultProgram)
        return false;
    SetUniform(g_DefaultProgram, "DiffuseSampler", 0);
    SetUniform(g_DefaultProgram, "LightDepthSampler", 1);
    const float shadowBias = GetConfigFloat("opengl.shadow-bias", 0.005);
    SetUniform(g_DefaultProgram, "ShadowBias", shadowBias);

    g_DepthTextureSize = GetConfigInt("opengl.shadowmap-size", 512);
    g_DepthTexture = CreateDepthTexture(g_DepthTextureSize, g_DepthTextureSize, TEX_CLAMP|TEX_FILTER|TEX_MIPMAP);
    if(!g_DepthTexture)
    {
        Error("Can't create depth texture!");
        return false;
    }

    glGenFramebuffers(1, &g_Framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, g_Framebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, g_DepthTexture, 0);
    glDrawBuffer(GL_NONE);
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        Error("Framebuffer incomplete!");
        return false;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // Switch back to the default framebuffer

    g_DepthProgram = LoadProgram("Shaders/Depth.vert", "Shaders/Depth.frag");
    if(!g_DepthProgram)
        return false;
    SetUniform(g_DepthProgram, "DiffuseSampler", 0);
    const float alphaThreshold = GetConfigFloat("opengl.alpha-threshold", 1);
    SetUniform(g_DepthProgram, "AlphaThreshold", alphaThreshold);

    SetLight(
        glm::vec3(0.1f, 0.1f, 0.1f),
        glm::vec3(0.9f, 0.9f, 0.9f),
        glm::vec3(0.5,2,2)
    );

    return true;
}

void DestroyEffects()
{
    FreeProgram(g_DefaultProgram);
    FreeTexture(g_DepthTexture);
    glDeleteFramebuffers(1, &g_Framebuffer);
}

void SetModelViewProjectionMatrix( Program program, const glm::mat4* mvpMatrix )
{
    SetUniformMatrix4(program, "MVP", mvpMatrix);
}

Program GetDefaultProgram()
{
    return g_DefaultProgram;
}

void UpdateDepthModelViewProjectionMatrix()
{
    using namespace glm;

    const mat4 projectionMatrix = ortho<float>(
        -5,5, // left / right
        -5,5, // bottom / top
        -10,20  // near / far
    );
    const mat4 viewMatrix = lookAt(g_LightDirection, vec3(0,0,0), vec3(0,1,0));
    //const mat4 modelMatrix = mat4();
    g_DepthMVP = projectionMatrix * viewMatrix; // * modelMatrix;
}

void BeginRenderShadowTexture()
{
    using namespace glm;

    UpdateDepthModelViewProjectionMatrix();

    BindProgram(g_DepthProgram);
    SetModelViewProjectionMatrix(g_DepthProgram, &g_DepthMVP);
    glBindFramebuffer(GL_FRAMEBUFFER, g_Framebuffer);
    glViewport(0, 0, g_DepthTextureSize, g_DepthTextureSize);
    glDisable(GL_CULL_FACE);
}

void EndRenderShadowTexture()
{
    glEnable(GL_CULL_FACE);
    const glm::ivec2 fbSize = GetFramebufferSize();
    glViewport(0, 0, fbSize.x, fbSize.y);
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // Switch back to the default framebuffer
}

void BeginRender()
{
    using namespace glm;

    BindProgram(g_DefaultProgram);
    BindTexture(GL_TEXTURE_2D, g_DepthTexture, 1);

    const glm::mat4 depthBiasMVP = BIAS_MATRIX * g_DepthMVP;
    SetUniformMatrix4(g_DefaultProgram, "DepthBiasMVP", &depthBiasMVP);
}

void EndRender()
{
}

void SetLight( glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 direction )
{
    direction = glm::normalize(direction);

    g_AmbientLight = ambient;
    g_DiffuseLight = diffuse;
    g_LightDirection = direction;

    SetUniform(g_DefaultProgram, "LightAmbient", 3, &ambient.x);
    SetUniform(g_DefaultProgram, "LightDiffuse", 3, &diffuse.x);
    SetUniform(g_DefaultProgram, "LightDirection", 3, &direction.x);
}