#include "Common.h"
#include "Window.h"
#include "OpenGL.h"
#include "Game.h"

namespace Game
{

void OnWindowResize( int width, int height )
{
    printf("window resize\n");
}

void OnMouseButtonAction( int button, bool pressed )
{
    printf("mouse button action\n");
}

void OnMouseScroll( double xoffset, double yoffset )
{
    printf("mouse scroll\n");
}

void OnMouseMove( double x, double y, double xoffset, double yoffset )
{
    printf("mouse move\n");
}

void OnKeyAction( int key, bool pressed )
{
    printf("key action\n");
}

int Loop()
{
    Window::Init("Apoapsis", 800, 600);
    while(!Window::ShouldClose())
    {
        Window::SwapBuffers();
    }
    Window::Deinit();
    return 0;
}

}
