#include "Common.h"
#include "Math.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Config.h"
#include "Window.h"
#include "Controls.h"
#include "OpenGL.h"
#include "Background.h"
#include "Vertex.h"
#include "Audio.h"
#include "Player.h"
#include "Map.h"
#include "Debug.h"
#include "Squirrel.h"
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

    Log("--------- Background ----------");
    if(!InitBackground())
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
    DestroySquirrel();
    DestroyMap();
    DestroyPlayer();
    DestroyBackground();
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

        // Render background
        glClear(GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        RotateWorld();
        DrawBackground();

        // Render map
        glClear(GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        RotateWorld();
        TranslateWorld();
        DrawMap();
        DrawPlayer();

        // Render HUD
        //glClear(GL_DEPTH_BUFFER_BIT);
        //glLoadIdentity();

        SwapBuffers();
    }
}

void OnFramebufferResize( int width, int height )
{
    using namespace glm;

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    const float aspect = float(width) / float(height);
    const mat4 perspectivicMatrix = perspective(glm::radians(90.0f), aspect, 0.1f, 100.0f);
    glLoadMatrixf(value_ptr(perspectivicMatrix));

    glMatrixMode(GL_MODELVIEW);
}

void OnExitKey( const char* name, bool pressed, void* context )
{
    if(pressed)
        FlagWindowForClose();
}

