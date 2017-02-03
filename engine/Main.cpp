#include <stdlib.h> // EXIT_FAILURE, EXIT_SUCCESS
#include <string.h> // strcmp, strncpy
#include <stdio.h> // printf

#include "Common.h"
#include "Math.h"
#include "Crc32.h"
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

#include "lua_bindings/Audio.h"
#include "lua_bindings/Camera.h"
#include "lua_bindings/Config.h"
#include "lua_bindings/Controls.h"
#include "lua_bindings/Image.h"
#include "lua_bindings/Math.h"
#include "lua_bindings/Mesh.h"
#include "lua_bindings/MeshBuffer.h"
#include "lua_bindings/MeshChunkGenerator.h"
#include "lua_bindings/RenderManager.h"
#include "lua_bindings/RenderTarget.h"
#include "lua_bindings/ModelWorld.h"
#include "lua_bindings/LightWorld.h"
#include "lua_bindings/PhysFS.h"
#include "lua_bindings/PhysicsManager.h"
#include "lua_bindings/Shader.h"
#include "lua_bindings/Time.h"
#include "lua_bindings/Texture.h"
#include "lua_bindings/VoxelVolume.h"


static int Lua_StopSimulation( lua_State* l )
{
    FlagWindowForClose();
    return 0;
}

static bool RegisterAllModulesInLua()
{
    return
        RegisterAudioInLua() &&
        RegisterCameraInLua() &&
        RegisterConfigInLua() &&
        RegisterControlsInLua() &&
        RegisterImageInLua() &&
        RegisterMathInLua() &&
        RegisterMeshInLua() &&
        RegisterMeshBufferInLua() &&
        RegisterMeshChunkGeneratorInLua() &&
        RegisterRenderManagerInLua() &&
        RegisterRenderTargetInLua() &&
        RegisterModelWorldInLua() &&
        RegisterLightWorldInLua() &&
        RegisterPhysFSInLua() &&
        RegisterPhysicsManagerInLua() &&
        RegisterShaderInLua() &&
        RegisterTimeInLua() &&
        RegisterTextureInLua() &&
        RegisterVoxelVolumeInLua() &&
        RegisterFunctionInLua("StopSimulation", Lua_StopSimulation);
}

static bool InitModules( const char* arg0 )
{
    InitCrc32();

    Log("------------- PhysFS -------------");
    if(!InitPhysFS(arg0))
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

    Log("--- Registering Lua modules ----");
    if(!RegisterAllModulesInLua())
        return false;

    Log("-------------------------------");
    return true;
}

static void DestroyModules()
{
    DestroyLua();
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

static bool InitScript( const char* scenarioPackage )
{
    return MountPackage("core") && RunLuaScript(GetLuaState(), "core/bootstrap/init.lua");
}

static void RunSimulation()
{
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
}


// --- Program arguments ---

static void PrintHelp( const char* arg0 )
{
    printf("Usage: %s [options] <packages>\n"
           "\n"
           "Options:\n"
           "\t--state=...        - Directory which stores the scenario state.\n"
           "\t--shared-state=... - Directory that is shared by all/multiple scenarios.\n"
           "\t--search-paths=...;...;... - Directories that are used to find packages.\n"
           "\t--config=...       - Read the given config file.\n"
           "\t-D<key>=<value>    - Set a config value.\n"
           "\t--help             - Show this help message.\n"
           "\n"
           "Packages:\n"
           "\tPackages must be passed either using their base name or file path.\n"
           "\tA base name looks like this: `<name>.<major>.<minor>.<patch>`\n", arg0);
}

static void ParseConfigString( const char* value )
{
    static char key[128];
    int i = 0;
    char c;

    for(;; i++)
    {
        if(i > 127)
            return;

        c = value[i];
        if(c == '=' ||
           c == '\0')
        {
            strncpy(key, value, i-1);
            break;
        }
    }

    if(c == '=')
        SetConfigString(key, &value[i+1]);
    else
        SetConfigBool(key, true);
}

static const char* MatchPrefix( const char* prefix, const char* value )
{
    const size_t prefixLength = strlen(prefix);
    if(strncmp(prefix, value, prefixLength) == 0)
        return &value[prefixLength];
    else
        return NULL;
}

struct Arguments
{
    const char* state;
    const char* sharedState;
    const char* searchPaths;
    const char* scenario;
};

static bool ParseArguments( const int argc, char** argv, Arguments* out )
{
    for(int i = 1; i < argc; i++)
    {
        const char* arg = argv[i];
        const char* match;

        match = MatchPrefix("--state=", arg);
        if(match) { out->state = match; continue; }

        match = MatchPrefix("--shared-state=", arg);
        if(match) { out->sharedState = match; continue; }

        match = MatchPrefix("--search-paths=", arg);
        if(match) { out->searchPaths = match; continue; }

        match = MatchPrefix("--config=", arg);
        if(match) { ReadConfigFile(match); continue; }

        match = MatchPrefix("-D", arg);
        if(match) { ParseConfigString(match); continue; }

        match = MatchPrefix("--help", arg);
        if(match) { PrintHelp(argv[0]); return false; }

        match = MatchPrefix("-", arg);
        if(match) { Error("Unknown argument '%s'", arg); return false; }

        // TODO: Mount package `arg`
        Log("Mounting '%s' ...", arg);
        if(!out->scenario) // first package is the scenario
            out->scenario = arg;
    }

    if(!out->scenario)
    {
        Error("Needs at least one package to use as scenario!");
        return false;
    }

    return true;
}

int main( const int argc, char** argv )
{
    Arguments args;
    memset(&args, 0, sizeof(args));
    if(!InitConfig() ||
       !ParseArguments(argc, argv, &args) ||
       !InitModules(argv[0]) ||
       !InitScript(args.scenario))
        return EXIT_FAILURE;
    RunSimulation();
    DestroyModules();
    return EXIT_SUCCESS;
}
