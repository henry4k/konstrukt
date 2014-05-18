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
#include "Debug.h"
#include "Lua.h"
#include "Effects.h"
#include "PhysicsManager.h"
#include "RenderManager.h"
#include "Game.h"


#include "Mesh.h"
#include "MeshBuffer.h"

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

    Log("--------- Physics Manager ----------");
    if(!InitPhysicsManager())
        return false;

    Log("--------- Render Manager ----------");
    if(!InitRenderManager())
        return false;

    /*
    Log("--------- Effects ----------");
    if(!InitEffects())
        return false;
    */

    Log("----------- Player ------------");
    if(!InitPlayer())
        return false;

    SetFrambufferFn(OnFramebufferResize);

    Log("-------------------------------");

    int r = luaL_dofile(GetLuaState(), "Scripts/Main.lua");
    if(r != LUA_OK)
        Error("%s", lua_tostring(GetLuaState(), -1));

    return true;
}

void DestroyGame()
{
    DestroyLua();
    DestroyPlayer();
    //DestroyEffects();
    DestroyRenderManager();
    DestroyPhysicsManager();
    DestroyControls();
    DestroyAudio();
    DestroyDebug();
    DestroyWindow();
    DestroyConfig();
}

void RunGame()
{
    using namespace glm;

    const Vertex v[] =
    {
        { vec3(0,0,5), vec3(0,0,0), vec2(0,0), vec3(0,0,0), vec4(0,0,0,0) },
        { vec3(0,1,5), vec3(0,0,0), vec2(0,0), vec3(0,0,0), vec4(0,0,0,0) },
        { vec3(1,1,5), vec3(0,0,0), vec2(0,0), vec3(0,0,0), vec4(0,0,0,0) }
    };

    MeshBuffer meshBuffer;
    CreateMeshBuffer(&meshBuffer);
    meshBuffer.vertices.push_back(v[0]);
    meshBuffer.vertices.push_back(v[1]);
    meshBuffer.vertices.push_back(v[2]);
    meshBuffer.indices.push_back(0);
    meshBuffer.indices.push_back(1);
    meshBuffer.indices.push_back(2);

    Mesh mesh;
    CreateMesh(&mesh, &meshBuffer);

    GraphicsObject* o = CreateGraphicsObject();
    o->mesh = &mesh;

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
        DrawGraphicsObjects(&mvpMatrix);

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

    FreeGraphicsObject(o);
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

