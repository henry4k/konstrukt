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
    //glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    glClearColor(0.5, 0.5, 0.5, 1);

    Log("------------ Audio ------------");
    if(!InitAudio())
        return false;

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

    return true;
}

void DestroyGame()
{
    DestroyTest();
    DestroyMap();
    DestroyPlayer();
    DestroyBackground();
    DestroyControls();
    DestroyAudio();
    DestroyWindow();
    DestroyConfig();
}

void ManualLoopOnStop( AudioSource source, void* context )
{
    const AudioBuffer buffer = *(AudioBuffer*)context;
    EnqueueAudioBuffer(source, buffer);
    PlayAudioSource(source);
    // TODO: This is buggy! Rework it. :O
}

void RunGame()
{
    using namespace glm;

    AudioBuffer audioBuffer = LoadAudioBuffer("Audio/Test.flac");
    AudioSource audioSource = CreateAudioSource(ManualLoopOnStop, &audioBuffer);

    EnqueueAudioBuffer(audioSource, audioBuffer);
    //PlayAudioSource(audioSource);


    double lastTime = glfwGetTime();
    while(!WindowShouldClose())
    {
        // Simulation
        const double curTime = glfwGetTime();
        const double timeDelta = curTime-lastTime;
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

        // Render HUD
        //glClear(GL_DEPTH_BUFFER_BIT);
        //glLoadIdentity();
        //DrawPlayer();

        SwapBuffers();
    }

    FreeAudioSource(audioSource);
    FreeAudioBuffer(audioBuffer);
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
