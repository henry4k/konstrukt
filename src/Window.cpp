#include <stdio.h>
#include <assert.h>

#include "Common.h"
#include "OpenGL.h"
#include "Game.h"
#include "Window.h"

GLFWwindow* g_Window = NULL;
bool g_WindowShouldClose = false;
int g_WindowWidth = 0;
int g_WindowHeight = 0;
double g_LastMouseX;
double g_LastMouseY;

void GLFW_OnError( int code, const char* description );
void GLFW_OnResize( GLFWwindow* window, int width, int height );
void GLFW_OnMouseButtonAction( GLFWwindow* window, int button, int action );
void GLFW_OnMouseScroll( GLFWwindow* window, double xoffset, double yoffset );
void GLFW_OnMouseMove( GLFWwindow* window, double x, double y );
void GLFW_OnKeyAction( GLFWwindow* window, int key, int action );

void InitWindow( const char* name, int width, int height )
{
    glfwSetErrorCallback(GLFW_OnError);
    if(!glfwInit())
        FatalError("GLFW initialization failed.");

    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    g_Window = glfwCreateWindow(width, height, name, NULL, NULL);
    if(!g_Window)
        FatalError("Window creation failed.");

    glfwGetCursorPos(g_Window, &g_LastMouseX, &g_LastMouseY);

    glfwMakeContextCurrent(g_Window);

    glfwSwapInterval(1); // enable vsync

    glfwSetInputMode(g_Window, GLFW_CURSOR_MODE, GLFW_CURSOR_NORMAL);
    glfwSetInputMode(g_Window, GLFW_STICKY_KEYS, GL_FALSE);
    glfwSetInputMode(g_Window, GLFW_STICKY_MOUSE_BUTTONS, GL_FALSE);

    glfwSetWindowSizeCallback(g_Window, GLFW_OnResize);
    glfwSetMouseButtonCallback(g_Window, GLFW_OnMouseButtonAction);
    glfwSetScrollCallback(g_Window, GLFW_OnMouseScroll);
    glfwSetCursorPosCallback(g_Window, GLFW_OnMouseMove);
    glfwSetKeyCallback(g_Window, GLFW_OnKeyAction);
}

void DeinitWindow()
{
    glfwTerminate();
}

void SwapBuffers()
{
    glfwSwapBuffers(g_Window);
    glfwPollEvents();
}

bool WindowShouldClose()
{
    return g_WindowShouldClose || glfwWindowShouldClose(g_Window) == GL_TRUE;
}

void GLFW_OnError( int code, const char* description )
{
    Error("GLFW error %d: %s", code, description);
}

void GLFW_OnResize( GLFWwindow* window, int width, int height )
{
    g_WindowWidth = width;
    g_WindowHeight = height;

    glViewport(0, 0, width, height);

    OnWindowResize(width, height);
}

void GLFW_OnMouseButtonAction( GLFWwindow* window, int button, int action )
{
    OnMouseButtonAction(button, action!=GLFW_RELEASE);
}

void GLFW_OnMouseScroll( GLFWwindow* window, double xoffset, double yoffset )
{
    OnMouseScroll(xoffset, yoffset);
}

void GLFW_OnMouseMove( GLFWwindow* window, double x, double y )
{
    const double dx = x - g_LastMouseX;
    const double dy = y - g_LastMouseY;

    g_LastMouseX = x;
    g_LastMouseY = y;

    OnMouseMove(x, y, dx, dy);
}

void GLFW_OnKeyAction( GLFWwindow* window, int key, int action )
{
    // Callback didn't handle the event.
    if(key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE)
        g_WindowShouldClose = true;

    OnKeyAction(key, action!=GLFW_RELEASE);
}
