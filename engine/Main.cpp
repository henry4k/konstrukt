#include <stdlib.h> // exit, EXIT_FAILURE, EXIT_SUCCESS
#include <string.h> // strcmp, strncpy
#include <stdio.h> // printf

#include "Profiler.h"
#include "Array.h"
#include "Common.h"
#include "Math.h"
#include "Crc32.h"
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
#include "Vfs.h"
#include "JobManager.h"

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
#include "lua_bindings/PhysicsManager.h"
#include "lua_bindings/Shader.h"
#include "lua_bindings/Time.h"
#include "lua_bindings/Texture.h"
#include "lua_bindings/VoxelVolume.h"
#include "lua_bindings/Vfs.h"


struct Arguments
{
    const char* state;
    const char* sharedState;
    Array<const char*> searchPaths;
    Array<const char*> packages;
};

static JobManager* JobManagerInstance = NULL;


static int Lua_StopSimulation( lua_State* l )
{
    FlagWindowForClose();
    return 0;
}

static void RegisterAllModulesInLua()
{
    RegisterAudioInLua();
    RegisterCameraInLua();
    RegisterConfigInLua();
    RegisterControlsInLua();
    RegisterImageInLua();
    RegisterMathInLua();
    RegisterMeshInLua();
    RegisterMeshBufferInLua();
    RegisterMeshChunkGeneratorInLua();
    RegisterRenderManagerInLua();
    RegisterRenderTargetInLua();
    RegisterModelWorldInLua();
    RegisterLightWorldInLua();
    RegisterPhysicsManagerInLua();
    RegisterShaderInLua();
    RegisterTimeInLua();
    RegisterTextureInLua();
    RegisterVoxelVolumeInLua();
    RegisterVfsInLua();
    RegisterFunctionInLua("StopSimulation", Lua_StopSimulation);
}

static void InitModules( const char* arg0, const Arguments* arguments )
{
    InitCrc32();
    InitProfiler();
    InitCommon();
    JobManagerInstance = CreateJobManager({3});
    InitWindow();
    InitGPUProfiler();
    InitVfs(arg0, arguments->state, arguments->sharedState);
    InitLua();
    InitTime();
    InitAudio();
    InitControls();
    InitPhysicsManager();
    InitShader();
    InitRenderManager();
    InitDefaultRenderTarget();
    RegisterAllModulesInLua();
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
    DestroyConfig();
    DestroyVfs();
    DestroyGPUProfiler();
    DestroyWindow();
    DestroyJobManager(JobManagerInstance);
    DestroyCommon();
    DestroyProfiler();
}

static void InitScript( const Arguments* args )
{
    REPEAT(args->searchPaths.length, i)
    {
        const char* searchPath = args->searchPaths.data[i];
        AddPackageSearchPath(searchPath);
    }

    MountPackage("core");
    REPEAT(args->packages.length, i)
    {
        const char* package = args->packages.data[i];
        MountPackage(package);
    }

    if(args->packages.length >= 1)
    {
        lua_pushstring(GetLuaState(), args->packages.data[0]);
        lua_setglobal(GetLuaState(), "_scenario");
    }

    RunLuaScript(GetLuaState(), "core/bootstrap/init.lua");
}

static void RunSimulation()
{
    LockJobManager(JobManagerInstance);

    double lastTime = glfwGetTime();
    while(!WindowShouldClose())
    {
        ProfileScope("Simulation");
        const double curTime = glfwGetTime();
        const double timeDelta = curTime-lastTime;
        UpdateTime(timeDelta);
        UpdateControls(timeDelta);
        UpdatePhysicsManager(timeDelta);
        lastTime = curTime;
        RenderScene();
        NotifyProfilerAboutStepCompletion();


        // (gesynctes zeugs hier tun)

        BeginLuaUpdate(JobManagerInstance);
        BeginAudioUpdate(JobManagerInstance);

        CompleteLuaUpdate(JobManagerInstance);
        CompleteAudioUpdate(JobManagerInstance);
    }

    UnlockJobManager(JobManagerInstance);
}


// --- Program arguments ---

static void PrintHelpAndExit( const char* arg0 )
{
    printf("Usage: %s [options] <scenario package> <other packages ...>\n"
           "\n"
           "Options:\n"
           "\t--state=...        - Directory which stores the scenario state.\n"
           "\t--shared-state=... - Directory that is shared by all/multiple scenarios.\n"
           "\t-I...              - Add a package search path.\n"
           "\t--config=...       - Read the given config file.\n"
           "\t-D<key>=<value>    - Set a config value.\n"
           "\t--help             - Show this help message.\n"
           "\n"
           "Packages:\n"
           "\tPackages must be passed either using their base name or file path.\n"
           "\tA base name looks like this: `<name>.<major>.<minor>.<patch>`\n"
           "\tThe first package is used as scenario.\n", arg0);
    exit(EXIT_FAILURE);
}

static void ParseConfigString( const char* value )
{
    static const int KEY_SIZE = 128;

    static char key[KEY_SIZE];
    int i = 0;
    char c;

    for(;; i++)
    {
        if(i > KEY_SIZE-1)
            return;

        c = value[i];
        if(c == '=' ||
           c == '\0')
        {
            strncpy(key, value, i);
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

static void ParseArguments( const int argc, char** argv, Arguments* out )
{
    if(argc == 1)
        PrintHelpAndExit(argv[0]);

    for(int i = 1; i < argc; i++)
    {
        const char* arg = argv[i];
        const char* match;

        match = MatchPrefix("--help", arg);
        if(match) { PrintHelpAndExit(argv[0]); }

        match = MatchPrefix("--state=", arg);
        if(match) { out->state = match; continue; }

        match = MatchPrefix("--shared-state=", arg);
        if(match) { out->sharedState = match; continue; }

        match = MatchPrefix("-I", arg);
        if(match) { AppendToArray(&out->searchPaths, 1, &match); continue; }

        match = MatchPrefix("--config=", arg);
        if(match) { ReadConfigFile(match, true); continue; }

        match = MatchPrefix("-D", arg);
        if(match) { ParseConfigString(match); continue; }

        match = MatchPrefix("-", arg);
        if(match) { FatalError("Unknown argument '%s'", arg); }

        AppendToArray(&out->packages, 1, &arg);
    }
}

int KonstruktMain( const int argc, char** argv )
{
    InitConfig();
    PostConfigInitLog();

    Arguments args;
    memset(&args, 0, sizeof(args));
    InitArray(&args.searchPaths);
    InitArray(&args.packages);

    ParseArguments(argc, argv, &args);

    InitModules(argv[0], &args);
    InitScript(&args);

    DestroyArray(&args.searchPaths);
    DestroyArray(&args.packages);

    RunSimulation();
    DestroyModules();
    return EXIT_SUCCESS;
}
