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
#include "Map.h"
#include "Debug.h"
#include "Squirrel.h"
#include "Lua.h"
#include "Effects.h"
#include "Game.h"

void OnFramebufferResize( int width, int height );
void OnExitKey( const char* name, bool pressed, void* context );

bool InitGame( const int argc, char** argv )
{
    Log("----------- Config ------------");
    if(!InitConfig(argc, argv))
    return false;

    Log("------------ Debug -------------");
    if(!InitDebug())
        return false;

    Log("------------- Lua -------------");
    if(!InitLua())
        return false;

    Log("---------- Squirrel -----------");
    if(!InitSquirrel())
        return false;

    Log("----------- Window ------------");
    if(!InitWindow())
        return false;

    EnableVertexArrays();
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.5, 0.5, 0.5, 1);

    if(!InitDebugGraphics())
        return false;

    Log("------------ Audio ------------");
    if(!InitAudio())
        return false;

    Log("---------- Controls -----------");
    if(!InitControls())
        return false;

    if(!RegisterKeyControl("exit", OnExitKey, NULL, NULL))
        return false;

    Log("--------- Effects ----------");
    if(!InitEffects())
        return false;

    Log("----------- Player ------------");
    if(!InitPlayer())
        return false;

    Log("----------- Map ------------");
    if(!InitMap())
        return false;


    SetFrambufferFn(OnFramebufferResize);


    Log("--------- Core Script ----------");
    if(!RunSquirrelFile("Scripts/Core.nut"))
        return false;

    Log("-------------------------------");


    RunSquirrelFile("Scripts/Test.nut");

    return true;
}

void DestroyGame()
{
    DestroyLua();
    DestroySquirrel();
    DestroyMap();
    DestroyPlayer();
    DestroyEffects();
    DestroyControls();
    DestroyAudio();
    DestroyDebug();
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
        UpdateSquirrel(timeDelta);
        UpdateAudio(timeDelta);
        UpdateControls(timeDelta);
        UpdatePlayer(timeDelta);
        lastTime = curTime;

        const glm::mat4 mvpMatrix =
            GetPlayerProjectionMatrix() *
            GetPlayerViewMatrix();

        // Render background
        //glClear(GL_DEPTH_BUFFER_BIT);
        //DrawBackground();

        // Render shadow map
        BeginRenderShadowTexture();
        glClear(GL_DEPTH_BUFFER_BIT);
        DrawMap();
        DrawPlayer();
        EndRenderShadowTexture();

        // Render world
        BeginRender();
        glClear(GL_DEPTH_BUFFER_BIT);
        BindProgram(GetDefaultProgram());
        SetModelViewProjectionMatrix(GetDefaultProgram(), &mvpMatrix);
        DrawMap();
        DrawPlayer();
        EndRender();

        // Render HUD
        //glClear(GL_DEPTH_BUFFER_BIT);
        //glLoadIdentity();

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

