#include "Common.h"
#include "Math.h"
#include "PhysFS.h"
#include "Config.h"
#include "Window.h"
#include "Controls.h"
#include "OpenGL.h"
#include "Vertex.h"
#include "Audio.h"
#include "Math.h"
#include "Lua.h"
#include "PhysicsManager.h"
#include "RenderTarget.h"
#include "RenderManager.h"
#include "Camera.h"
#include "Shader.h"
#include "Time.h"
#include "VoxelVolume.h"
#include "Game.h"

#include "lua_bindings/Audio.h"
#include "lua_bindings/Camera.h"
#include "lua_bindings/Config.h"
#include "lua_bindings/Controls.h"
#include "lua_bindings/ChunkGenerator.h"
#include "lua_bindings/Math.h"
#include "lua_bindings/Mesh.h"
#include "lua_bindings/MeshBuffer.h"
#include "lua_bindings/RenderManager.h"
#include "lua_bindings/RenderTarget.h"
#include "lua_bindings/ModelWorld.h"
#include "lua_bindings/PhysFS.h"
#include "lua_bindings/PhysicsManager.h"
#include "lua_bindings/Shader.h"
#include "lua_bindings/Time.h"
#include "lua_bindings/Texture.h"
#include "lua_bindings/VoxelVolume.h"


static int Lua_StopGameLoop( lua_State* l );

static bool RegisterAllModulesInLua();

static void CreateArgumentTable( const int argc, char** argv );

bool InitGame( const int argc, char** argv )
{
    Log("------------- PhysFS -------------");
    if(!InitPhysFS(argc, argv))
        return false;

    Log("----------- Config ------------");
    if(!InitConfig(argc, argv))
        return false;

    Log("----------- Log post config init ------------");
    if(!PostConfigInitLog())
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

    Log("--------- Time ----------");
    if(!InitTime())
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

    Log("--------- Voxel Volume ----------");
    if(!InitVoxelVolume())
        return false;

    Log("--- Registering Lua modules ----");
    if(!RegisterAllModulesInLua())
        return false;

    Log("-------------------------------");

    CreateArgumentTable(argc, argv);
    return MountPackage("core") && RunLuaScript(GetLuaState(), "core/bootstrap/init.lua");
}

static bool RegisterAllModulesInLua()
{
    return
        RegisterAudioInLua() &&
        RegisterCameraInLua() &&
        RegisterConfigInLua() &&
        RegisterControlsInLua() &&
        RegisterChunkGeneratorInLua() &&
        RegisterMathInLua() &&
        RegisterMeshInLua() &&
        RegisterMeshBufferInLua() &&
        RegisterRenderManagerInLua() &&
        RegisterRenderTargetInLua() &&
        RegisterModelWorldInLua() &&
        RegisterPhysFSInLua() &&
        RegisterPhysicsManagerInLua() &&
        RegisterShaderInLua() &&
        RegisterTimeInLua() &&
        RegisterTextureInLua() &&
        RegisterVoxelVolumeInLua() &&
        RegisterFunctionInLua("StopGameLoop", Lua_StopGameLoop);
}

static void DestroyGame()
{
    DestroyLua();
    DestroyVoxelVolume();
    DestroyDefaultRenderTarget();
    DestroyRenderManager();
    DestroyShader();
    DestroyPhysicsManager();
    DestroyControls();
    DestroyAudio();
    DestroyTime();
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
        UpdateTime(timeDelta);
        UpdateAudio();
        UpdateControls(timeDelta);
        UpdatePhysicsManager(timeDelta);
        lastTime = curTime;
        RenderScene();
    }

    DestroyGame();
}

static int Lua_StopGameLoop( lua_State* l )
{
    FlagWindowForClose();
    return 0;
}

static void CreateArgumentTable( const int argc, char** argv )
{
    lua_State* l = GetLuaState();
    lua_createtable(l, argc-1, 0);
    for(int i = 1; i < argc; i++)
    {
        const char* argument = argv[i];
        lua_pushstring(l, argument);
        lua_rawseti(l, -2, i);
    }
    lua_setglobal(l, "ARGS");
}

