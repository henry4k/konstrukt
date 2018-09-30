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
#include "PhysicsWorld.h"
#include "RenderTarget.h"
#include "RenderManager.h"
#include "Camera.h"
#include "Shader.h"
#include "Time.h"
#include "Vfs.h"
#include "JobManager.h"

#include "lua_bindings/Audio.h"
#include "lua_bindings/AttachmentTarget.h"
#include "lua_bindings/Camera.h"
#include "lua_bindings/Config.h"
#include "lua_bindings/Controls.h"
#include "lua_bindings/Image.h"
#include "lua_bindings/JobManager.h"
#include "lua_bindings/Math.h"
#include "lua_bindings/Mesh.h"
#include "lua_bindings/MeshBuffer.h"
#include "lua_bindings/MeshChunkGenerator.h"
#include "lua_bindings/RenderManager.h"
#include "lua_bindings/RenderTarget.h"
#include "lua_bindings/ModelWorld.h"
#include "lua_bindings/LightWorld.h"
#include "lua_bindings/LuaBuffer.h"
#include "lua_bindings/PhysicsWorld.h"
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


static int Lua_StopSimulation( lua_State* l )
{
    FlagWindowForClose();
    return 0;
}

static void RegisterAllModulesInLua()
{
    RegisterAudioInLua();
    RegisterAttachmentTargetInLua();
    RegisterCameraInLua();
    RegisterConfigInLua();
    RegisterControlsInLua();
    RegisterImageInLua();
    RegisterJobManagerInLua();
    RegisterMathInLua();
    RegisterMeshInLua();
    RegisterMeshBufferInLua();
    RegisterMeshChunkGeneratorInLua();
    RegisterRenderManagerInLua();
    RegisterRenderTargetInLua();
    RegisterModelWorldInLua();
    RegisterLightWorldInLua();
    RegisterLuaBufferInLua();
    RegisterPhysicsWorldInLua();
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
    InitJobManager({3});
    InitWindow();
    InitGPUProfiler();
    InitVfs(arg0, arguments->state, arguments->sharedState);
    InitLua();
    InitTime();
    InitAudio();
    InitControls();
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
    DestroyControls();
    DestroyAudio();
    DestroyTime();
    DestroyConfig();
    DestroyVfs();
    DestroyGPUProfiler();
    DestroyWindow();
    DestroyJobManager();
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

    // TODO: Run scenario initialization via event
    //if(args->packages.length >= 1)
    //{
    //    lua_pushstring(GetLuaState(), args->packages.data[0]);
    //    lua_setglobal(GetLuaState(), "_scenario");
    //}

    SetLuaWorkerCount(1);
}

static void RunSimulation()
{
    //LockJobManager();

    double lastTime = glfwGetTime();
    while(!WindowShouldClose())
    {
        ProfileScope("Simulation");
        const double curTime = glfwGetTime();
        const double timeDelta = curTime-lastTime;
        UpdateTime(timeDelta);
        UpdateControls(timeDelta);
        lastTime = curTime;

        // do serial stuff here

        BeginLuaUpdate();
        BeginAudioUpdate();
        BeginRenderManagerUpdate(NULL, timeDelta);

        CompleteLuaUpdate();
        CompleteAudioUpdate();
        CompleteRenderManagerUpdate(NULL);

        NotifyProfilerAboutStepCompletion();
    }

    //UnlockJobManager();
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
        if(match) { ReadConfigString(match); continue; }

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
