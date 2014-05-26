#include "Common.h"
#include "Math.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Config.h"
#include "Window.h"
#include "Controls.h"
#include "OpenGL.h"
#include "Vertex.h"
#include "Audio.h"
#include "Player.h"
#include "Math.h"
#include "Lua.h"
#include "PhysicsManager.h"
#include "RenderManager.h"
#include "Game.h"

#include "LuaBindings/Audio.h"
#include "LuaBindings/Config.h"
#include "LuaBindings/Controls.h"
#include "LuaBindings/Math.h"
#include "LuaBindings/Mesh.h"
#include "LuaBindings/MeshBuffer.h"
#include "LuaBindings/Player.h"
#include "LuaBindings/RenderManager.h"
#include "LuaBindings/Shader.h"


void OnFramebufferResize( int width, int height );
void OnExitKey( const char* name, bool pressed, void* context );
bool RegisterAllModulesInLua();

bool InitGame( const int argc, char** argv )
{
    Log("----------- Config ------------");
    if(!InitConfig(argc, argv))
        return false;

    Log("------------- Lua -------------");
    if(!InitLua())
        return false;
    if(!RegisterAllModulesInLua())
        return false;

    Log("----------- Window ------------");
    if(!InitWindow())
        return false;

    EnableVertexArrays();
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    //glEnable(GL_CULL_FACE);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.5, 0.5, 0.5, 1);

    Log("------------ Audio ------------");
    if(!InitAudio())
        return false;

    Log("---------- Controls -----------");
    if(!InitControls())
        return false;

    if(!RegisterKeyControl("exit", OnExitKey, NULL, NULL))
        return false;

    Log("--------- Physics Manager ----------");
    if(!InitPhysicsManager())
        return false;

    Log("--------- Render Manager ----------");
    if(!InitRenderManager())
        return false;

    Log("----------- Player ------------");
    if(!InitPlayer())
        return false;

    SetFrambufferFn(OnFramebufferResize);

    Log("-------------------------------");

    return RunLuaScript(GetLuaState(), "core/Main.lua");
}

bool RegisterAllModulesInLua()
{
    return
        RegisterAudioInLua() &&
        RegisterConfigInLua() &&
        RegisterControlsInLua() &&
        RegisterMathInLua() &&
        RegisterMeshInLua() &&
        RegisterMeshBufferInLua() &&
        RegisterPlayerInLua() &&
        RegisterRenderManagerInLua() &&
        RegisterShaderInLua();
}

void DestroyGame()
{
    DestroyLua();
    DestroyPlayer();
    DestroyRenderManager();
    DestroyPhysicsManager();
    DestroyControls();
    DestroyAudio();
    DestroyWindow();
    DestroyConfig();
}

void RunGame()
{
    using namespace glm;

    double lastTime = glfwGetTime();
    while(!WindowShouldClose())
    {
        // Simulation
        const double curTime = glfwGetTime();
        const double timeDelta = curTime-lastTime;
        //UpdateLua();
        UpdateAudio(timeDelta);
        UpdateControls(timeDelta);
        UpdatePlayer(timeDelta);
        lastTime = curTime;

        const glm::mat4 mvpMatrix =
            GetPlayerProjectionMatrix() *
            GetPlayerViewMatrix();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        DrawModels(mvpMatrix);

        /*
        // Render shadow map
        BeginRenderShadowTexture();
        glClear(GL_DEPTH_BUFFER_BIT);
        DrawGraphicsObjects();
        DrawPlayer();
        EndRenderShadowTexture();

        // Render world
        BeginRender();
        glClear(GL_DEPTH_BUFFER_BIT);
        BindProgram(GetDefaultProgram());
        SetModelViewProjectionMatrix(GetDefaultProgram(), &mvpMatrix);
        DrawGraphicsObjects();
        DrawPlayer();
        EndRender();

        // Render HUD
        glClear(GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        */

        SwapBuffers();
    }
}

void OnFramebufferResize( int width, int height )
{
    glViewport(0, 0, width, height);
    UpdateProjectionMatrix(width, height);
}

void OnExitKey( const char* name, bool pressed, void* context )
{
    if(pressed)
        FlagWindowForClose();
}

