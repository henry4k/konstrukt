#include <assert.h>
#include <stdlib.h> // NULL

#include "Common.h"
#include "Constants.h" // OS_CALLING_CONVENTION
#include "Config.h"
#include "OpenGL.h"
#include "Window.h"

#if defined(OS_CALLING_CONVENTION)
    #if defined(_MSC_VER) // using MSVC
        #define CALLBACK_API OS_CALLING_CONVENTION
    #else // using a GCC compatible compiler
        #define CALLBACK_API __attribute__((OS_CALLING_CONVENTION))
    #endif
#else
    #define CALLBACK_API
#endif


/****** Variables *******/

static GLFWwindow* g_Window = NULL;
static int g_WindowWidth  = 0;
static int g_WindowHeight = 0;
static int g_FramebufferWidth  = 0;
static int g_FramebufferHeight = 0;
static FramebufferResizeFn g_FramebufferResizeFn = NULL;
static CursorMoveFn        g_CursorMoveFn = NULL;
static MouseButtonActionFn g_MouseButtonActionFn = NULL;
static MouseScrollFn       g_MouseScrollFn = NULL;
static KeyActionFn         g_KeyActionFn = NULL;



/****** General *******/

static void OnGLFWError( int code, const char* description );
#if defined(KONSTRUKT_GL_DEBUG_FIX)
CALLBACK_API static void OnDebugEvent( GLenum source,
                                       GLenum type,
                                       GLuint id,
                                       GLenum severity,
                                       GLsizei length,
                                       const char* message,
                                       void* userParam );
#endif
CALLBACK_API static void OnDebugEvent( GLenum source,
                                       GLenum type,
                                       GLuint id,
                                       GLenum severity,
                                       GLsizei length,
                                       const char* message,
                                       const void* userParam );
static void OnWindowResize( GLFWwindow* window, int width, int height );
static void OnFramebufferResize( GLFWwindow* window, int width, int height );
static void OnMouseButtonAction( GLFWwindow* window, int button, int action, int mods );
static void OnMouseScroll( GLFWwindow* window, double xoffset, double yoffset );
static void OnCursorMove( GLFWwindow* window, double x, double y );
static void OnKeyAction( GLFWwindow* window, int key, int scancode, int action, int mods );

void InitWindow()
{
    const int width  = GetConfigInt("window.width",  640);
    const int height = GetConfigInt("window.height", 480);
    const char* title = GetConfigString("window.title", "Konstrukt");

    const bool debug = GetConfigBool("opengl.debug", false);
    const bool vsync = GetConfigBool("opengl.vsync", true);

    LogInfo("Compiled with GLFW %d.%d.%d",
             GLFW_VERSION_MAJOR,
             GLFW_VERSION_MINOR,
             GLFW_VERSION_REVISION);
    LogInfo("Using GLFW %s", glfwGetVersionString());

    assert(g_Window == NULL);
    glfwSetErrorCallback(OnGLFWError);
    if(!glfwInit())
        FatalError("GLFW init failed.");

    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DEPTH_BITS, GetConfigInt("opengl.depth-bits", 24));
    glfwWindowHint(GLFW_STENCIL_BITS, 0);
    glfwWindowHint(GLFW_SAMPLES, GetConfigInt("opengl.samples", 0));
    glfwWindowHint(GLFW_SRGB_CAPABLE, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, debug ? GL_TRUE : GL_FALSE);

    g_Window = glfwCreateWindow(width, height, title, NULL, NULL);
    if(!g_Window)
        FatalError("Window creation failed.");

    glfwGetWindowSize(g_Window, &g_WindowWidth, &g_WindowHeight);
    glfwGetFramebufferSize(g_Window, &g_FramebufferWidth, &g_FramebufferHeight);

    glfwMakeContextCurrent(g_Window);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        FatalError("Failed to load OpenGL.");

    LogInfo("Using OpenGL %s\n"
            "Vendor: %s\n"
            "Renderer: %s\n"
            "GLSL: %s",
            glGetString(GL_VERSION),
            glGetString(GL_VENDOR),
            glGetString(GL_RENDERER),
            glGetString(GL_SHADING_LANGUAGE_VERSION));

    if(vsync)
    {
        if(glfwExtensionSupported("GLX_EXT_swap_control_tear") ||
           glfwExtensionSupported("WGL_EXT_swap_control_tear"))
            glfwSwapInterval(-1); // enable vsync (allow the driver to swap even if a frame arrives a little bit late)
        else
            glfwSwapInterval(1); // enable vsync
    }
    else
    {
        glfwSwapInterval(0); // disable vsync
    }

    glfwSetInputMode(g_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetInputMode(g_Window, GLFW_STICKY_KEYS, GL_FALSE);
    glfwSetInputMode(g_Window, GLFW_STICKY_MOUSE_BUTTONS, GL_FALSE);

    if(debug)
    {
        if(!GLAD_GL_ARB_debug_output)
            FatalError("Debug output requested, but it's not supported!");

        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
        glDebugMessageCallbackARB(OnDebugEvent, NULL);
        LogInfo("Debug output supported! You may receive debug messages from your OpenGL driver.");
    }

    glfwSetWindowSizeCallback(g_Window, OnWindowResize);
    glfwSetFramebufferSizeCallback(g_Window, OnFramebufferResize);
    glfwSetMouseButtonCallback(g_Window, OnMouseButtonAction);
    glfwSetScrollCallback(g_Window, OnMouseScroll);
    glfwSetCursorPosCallback(g_Window, OnCursorMove);
    glfwSetKeyCallback(g_Window, OnKeyAction);
}

void DestroyWindow()
{
    assert(g_Window != NULL);
    glfwDestroyWindow(g_Window);
    glfwTerminate();
}

void SwapBuffers()
{
    glfwSwapBuffers(g_Window);
    glfwPollEvents();
}

void SetWindowTitle( const char* title )
{
    glfwSetWindowTitle(g_Window, title);
}

void FlagWindowForClose()
{
    glfwSetWindowShouldClose(g_Window, GL_TRUE);
}

bool WindowShouldClose()
{
    return glfwWindowShouldClose(g_Window) == GL_TRUE;
}

void* GetGLFWwindow()
{
    return g_Window;
}

void GetFramebufferSize( int* width, int* height )
{
    *width  = g_FramebufferWidth;
    *height = g_FramebufferHeight;
}


/****** Input ******/

void SetCursorMode( int mode )
{
    glfwSetInputMode(g_Window, GLFW_CURSOR, mode);
}


/******* Callback Setter *******/

void SetFrambufferFn( FramebufferResizeFn fn )
{
    g_FramebufferResizeFn = fn;
    OnFramebufferResize(NULL, g_FramebufferWidth, g_FramebufferHeight);
}

void SetCursorMoveFn( CursorMoveFn fn )
{
    g_CursorMoveFn = fn;
}

void SetMouseButtonActionFn( MouseButtonActionFn fn )
{
    g_MouseButtonActionFn = fn;
}

void SetMouseScrollFn( MouseScrollFn fn )
{
    g_MouseScrollFn = fn;
}

void SetKeyActionFn( KeyActionFn fn )
{
    g_KeyActionFn = fn;
}


/******* Callbacks ********/

static void OnGLFWError( int code, const char* description )
{
    FatalError("GLFW error %d: %s", code, description);
}

#if defined(KONSTRUKT_GL_DEBUG_FIX)
CALLBACK_API static void OnDebugEvent( GLenum source,
                                       GLenum type,
                                       GLuint id,
                                       GLenum severity,
                                       GLsizei length,
                                       const char* message,
                                       void* userParam )
{
    OnDebugEvent(source, type, id, severity, length, message, (const void*)userParam);
}
#endif

CALLBACK_API static void OnDebugEvent( GLenum source,
                                       GLenum type,
                                       GLuint id,
                                       GLenum severity,
                                       GLsizei length,
                                       const char* message,
                                       const void* userParam )
{
    const char* sourceName = "";
    switch(source)
    {
        case GL_DEBUG_SOURCE_API_ARB: sourceName = "API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB: sourceName = "window system"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB: sourceName = "shader"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY_ARB: sourceName = "third party"; break;
        case GL_DEBUG_SOURCE_APPLICATION_ARB: sourceName = "application"; break;
        case GL_DEBUG_SOURCE_OTHER_ARB: sourceName = ""; break;
    }

    LogLevel logLevel = LOG_WARNING;
    if(type == GL_DEBUG_TYPE_ERROR_ARB)
        logLevel = LOG_ERROR;

    const char* typeName = "unknown issue";
    switch(type)
    {
        case GL_DEBUG_TYPE_ERROR_ARB: typeName = "error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB: typeName = "deprecated behavior"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB: typeName = "undefined behavior"; break;
        case GL_DEBUG_TYPE_PORTABILITY_ARB: typeName = "partability issue"; break;
        case GL_DEBUG_TYPE_PERFORMANCE_ARB: typeName = "performance issue"; break;
        case GL_DEBUG_TYPE_OTHER_ARB: typeName = "unknown issue"; break;
    }

    const char* severityName = "";
    switch(severity)
    {
        case GL_DEBUG_SEVERITY_HIGH_ARB: severityName = "High"; break;
        case GL_DEBUG_SEVERITY_MEDIUM_ARB: severityName = "Medium"; break;
        case GL_DEBUG_SEVERITY_LOW_ARB: severityName = "Low"; break;
    }

    Log(logLevel, "%s severity %s %s %d, %s", severityName, sourceName, typeName, id, message);
}

static void OnWindowResize( GLFWwindow* window, int width, int height )
{
    g_WindowWidth = width;
    g_WindowHeight = height;
    // TODO: Update cursor position
}

static void OnFramebufferResize( GLFWwindow* window, int width, int height )
{
    g_FramebufferWidth = width;
    g_FramebufferHeight = height;
    // TODO: Update cursor position
    if(g_FramebufferResizeFn)
        g_FramebufferResizeFn(width, height);
}

static void OnMouseButtonAction( GLFWwindow* window, int button, int action, int mods )
{
    if(g_MouseButtonActionFn)
        g_MouseButtonActionFn(button, action==GLFW_PRESS);
}

static void OnMouseScroll( GLFWwindow* window, double xoffset, double yoffset )
{
    if(g_MouseScrollFn)
        g_MouseScrollFn(xoffset, yoffset);
}

static void OnCursorMove( GLFWwindow* window, double x, double y )
{
    x *= g_FramebufferWidth / g_WindowWidth;
    y *= g_FramebufferHeight / g_WindowHeight;

    if(g_CursorMoveFn)
        g_CursorMoveFn(x, y);
}

static void OnKeyAction( GLFWwindow* window, int key, int scancode, int action, int mods )
{
    if(action != GLFW_REPEAT && g_KeyActionFn)
        g_KeyActionFn(key, scancode, action==GLFW_PRESS);
}
