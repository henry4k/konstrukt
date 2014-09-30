#include "Common.h"
#include "Math.h"
#include "PhysFS.h"
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
#include "RenderTarget.h"
#include "RenderManager.h"
#include "Camera.h"
#include "Shader.h"
#include "Game.h"

#include "LuaBindings/Audio.h"
#include "LuaBindings/Camera.h"
#include "LuaBindings/Config.h"
#include "LuaBindings/Controls.h"
#include "LuaBindings/Math.h"
#include "LuaBindings/Mesh.h"
#include "LuaBindings/MeshBuffer.h"
#include "LuaBindings/Player.h"
#include "LuaBindings/RenderManager.h"
#include "LuaBindings/RenderTarget.h"
#include "LuaBindings/ModelWorld.h"
#include "LuaBindings/PhysFS.h"
#include "LuaBindings/PhysicsManager.h"
#include "LuaBindings/Shader.h"
#include "LuaBindings/Texture.h"


static int Lua_StopGameLoop( lua_State* l );

static bool RegisterAllModulesInLua();

bool InitGame( const int argc, char** argv )
{
    Log("------------- PhysFS -------------");
    if(!InitPhysFS(argc, argv))
        return false;

    Log("----------- Config ------------");
    if(!InitConfig(argc, argv))
        return false;

    Log("----------- PhysFS post config init ------------");
    if(!PostConfigInitPhysFS())
        return false;

    Log("------------- Lua -------------");
    if(!InitLua())
        return false;

    Log("----------- Window ------------");
    if(!InitWindow())
        return false;

    Log("------------ Audio ------------");
    if(!InitAudio())
        return false;

    Log("---------- Controls -----------");
    if(!InitControls())
        return false;

    Log("--------- Physics Manager ----------");
    if(!InitPhysicsManager())
        return false;

    Log("--------- Shader ----------");
    if(!InitShader())
        return false;

    Log("--------- Render Manager ----------");
    if(!InitRenderManager())
        return false;

    Log("--------- Default Render Target ----------");
    if(!InitDefaultRenderTarget())
        return false;

    Log("----------- Player ------------");
    if(!InitPlayer())
        return false;

    Log("--- Registering Lua modules ----");
    if(!RegisterAllModulesInLua())
        return false;

    Log("-------------------------------");

    if(!MountPackage("core") || !RunLuaScript(GetLuaState(), "/core/init.lua"))
        return false;

    for(int i = 1; i < argc; i++)
    {
        const char* argument = argv[i];
        if(argument[0] != '-')
            if(!RunLuaScript(GetLuaState(), argument))
                return false;
    }
    return true;
}

static bool RegisterAllModulesInLua()
{
    return
        RegisterAudioInLua() &&
        RegisterCameraInLua() &&
        RegisterConfigInLua() &&
        RegisterControlsInLua() &&
        RegisterMathInLua() &&
        RegisterMeshInLua() &&
        RegisterMeshBufferInLua() &&
        RegisterPlayerInLua() &&
        RegisterRenderManagerInLua() &&
        RegisterRenderTargetInLua() &&
        RegisterModelWorldInLua() &&
        RegisterPhysFSInLua() &&
        RegisterPhysicsManagerInLua() &&
        RegisterShaderInLua() &&
        RegisterTextureInLua() &&
        RegisterFunctionInLua("StopGameLoop", Lua_StopGameLoop);
}

static void DestroyGame()
{
    DestroyLua();
    DestroyPlayer();
    DestroyDefaultRenderTarget();
    DestroyRenderManager();
    DestroyShader();
    DestroyPhysicsManager();
    DestroyControls();
    DestroyAudio();
    DestroyWindow();
    DestroyConfig();
    DestroyPhysFS();
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
        UpdateAudio();
        UpdateControls(timeDelta);
        UpdatePlayer(timeDelta);
        UpdatePhysicsManager(timeDelta);
        lastTime = curTime;

        RenderTarget* defaultRenderTarget = GetDefaultRenderTarget();
        Camera* camera = GetRenderTargetCamera(defaultRenderTarget);
        if(camera)
            SetCameraViewTransformation(camera, GetPlayerViewMatrix());
        RenderScene();
    }

    DestroyGame();
}

static int Lua_StopGameLoop( lua_State* l )
{
    FlagWindowForClose();
    return 0;
}

