#include "Common.h"
#include "Window.h"
#include "OpenGL.h"
#include "Game.h"

void OnWindowResize( int width, int height )
{
    Log("window resize");
}

void OnMouseButtonAction( int button, bool pressed )
{
    Log("mouse button action");
}

void OnMouseScroll( double xoffset, double yoffset )
{
    Log("mouse scroll");
}

void OnMouseMove( double x, double y, double xoffset, double yoffset )
{
    Log("mouse move");
}

void OnKeyAction( int key, bool pressed )
{
    Log("key action");
}

int GameLoop()
{
    InitWindow("Apoapsis", 800, 600);
    while(!WindowShouldClose())
    {
        SwapBuffers();
    }
    DeinitWindow();
    return 0;
}
