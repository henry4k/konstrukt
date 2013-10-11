#include <stdio.h>
#include <assert.h>

#include "Common.h"
#include "OpenGL.h"
#include "Game.h"
#include "Window.h"

namespace Window
{

GLFWwindow* g_Window = NULL;
bool g_ShouldClose = false;
int g_Width = 0;
int g_Height = 0;
double g_LastMouseX;
double g_LastMouseY;

void GLFW_OnError( int code, const char* description );
void GLFW_OnResize( GLFWwindow* window, int width, int height );
void GLFW_OnMouseButtonAction( GLFWwindow* window, int button, int action );
void GLFW_OnMouseScroll( GLFWwindow* window, double xoffset, double yoffset );
void GLFW_OnMouseMove( GLFWwindow* window, double x, double y );
void GLFW_OnKeyAction( GLFWwindow* window, int key, int action );

void Init( const char* name, int width, int height )
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

void Deinit()
{
    glfwTerminate();
}

void SwapBuffers()
{
    glfwSwapBuffers(g_Window);
    glfwPollEvents();
}

bool ShouldClose()
{
    return g_ShouldClose || glfwWindowShouldClose(g_Window) == GL_TRUE;
}

void GLFW_OnError( int code, const char* description )
{
    fprintf(stderr, "GLFW error %d: %s\n", code, description);
}

void GLFW_OnResize( GLFWwindow* window, int width, int height )
{
    g_Width = width;
    g_Height = height;

    glViewport(0, 0, width, height);

    Game::OnWindowResize(width, height);
}

void GLFW_OnMouseButtonAction( GLFWwindow* window, int button, int action )
{
    Game::OnMouseButtonAction(button, action!=GLFW_RELEASE);
}

void GLFW_OnMouseScroll( GLFWwindow* window, double xoffset, double yoffset )
{
    Game::OnMouseScroll(xoffset, yoffset);
}

void GLFW_OnMouseMove( GLFWwindow* window, double x, double y )
{
    const double dx = x - g_LastMouseX;
    const double dy = y - g_LastMouseY;

    g_LastMouseX = x;
    g_LastMouseY = y;

    Game::OnMouseMove(x, y, dx, dy);
}

void GLFW_OnKeyAction( GLFWwindow* window, int key, int action )
{
    // Callback didn't handle the event.
    if(key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE)
        g_ShouldClose = true;

    Game::OnKeyAction(key, action!=GLFW_RELEASE);
}

}
