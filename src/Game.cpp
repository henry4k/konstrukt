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
#include "Player.h"
#include "Map.h"
#include "Test.h"
#include "Game.h"

void OnFramebufferResize( int width, int height );
void OnExitKey( const char* name, bool pressed );

bool InitGame( const int argc, char** argv )
{
    Log("----------- Config ------------");
    if(!InitConfig(argc, argv))
    return false;

    Log("----------- Window ------------");
    if(!InitWindow())
        return false;

    EnableVertexArrays();
    glEnable(GL_DEPTH_TEST);

    Log("---------- Controls -----------");
    if(!InitControls())
        return false;

    if(!RegisterKeyControl("exit", OnExitKey, NULL))
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

    Log("----------- Test-Game ------------");
    if(!InitTest())
        return false;

    Log("-------------------------------");

    SetFrambufferFn(OnFramebufferResize);
    glEnable(GL_CULL_FACE);

    return true;
}

void DestroyGame()
{
    DestroyTest();
    DestroyMap();
    DestroyPlayer();
    DestroyBackground();
    DestroyControls();
    DestroyWindow();
    DestroyConfig();
}

void RunGame()
{
    using namespace glm;

    glClearColor(0.5, 0.5, 0.5, 1);

    double lastTime = glfwGetTime();
    while(!WindowShouldClose())
    {
        const mat4 lookAtMatrix = lookAt(
            vec3(4,4,4),
            vec3(0,0,0),
            vec3(0,1,0)
        );

        // Simulation
        const double curTime = glfwGetTime();
        const double timeDelta = curTime-lastTime;
        UpdateControls(timeDelta);
        UpdatePlayer(timeDelta);
        lastTime = curTime;

        // Render world
        glClear(GL_DEPTH_BUFFER_BIT);
        glLoadMatrixf(value_ptr(lookAtMatrix));
        RotateWorld();
        TranslateWorld();
        DrawMap();
        DrawBackground();

        // Render HUD
        //glClear(GL_DEPTH_BUFFER_BIT);
        //glLoadIdentity();
        //DrawPlayer();

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

void OnExitKey( const char* name, bool pressed )
{
    if(pressed)
        FlagWindowForClose();
}
