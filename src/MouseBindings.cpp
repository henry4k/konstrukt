#include <string.h>
#include <vector>
#include <map>
#include "Common.h"
#include "Config.h"
#include "OpenGL.h"
#include "Window.h"
#include "Controls.h"

void OnCursorMove( double x, double y );
void OnMouseButtonAction( int button, bool pressed );
void OnMouseScroll( double xoffset, double yoffset );

void OnCursorGrabEvent( const char* name, bool pressed, void* context );

struct MouseButtonBinding
{
    bool isEnabled;
    int keyControl;
    // button: see OnMouseButtonAction
};

struct MouseAxisBinding
{
    bool isEnabled;
    int axisControl;
    // axis: see OnCursorMove
};

static const int MOUSE_BUTTON_COUNT = GLFW_MOUSE_BUTTON_LAST+1;

enum MouseAxis
{
    MOUSE_AXIS_X = 0,
    MOUSE_AXIS_Y,
    MOUSE_SCROLL_AXIS_X,
    MOUSE_SCROLL_AXIS_Y,
    MOUSE_AXIS_COUNT
};

MouseButtonBinding g_MouseButtonBindings[MOUSE_BUTTON_COUNT];
MouseAxisBinding   g_MouseAxisBindings[MOUSE_AXIS_COUNT];

bool g_CursorGrabbed;


bool InitMouseBindings()
{
    SetCursorMoveFn(OnCursorMove);
    SetMouseButtonActionFn(OnMouseButtonAction);
    SetMouseScrollFn(OnMouseScroll);

    memset(g_MouseButtonBindings, 0, sizeof(g_MouseButtonBindings));
    memset(g_MouseAxisBindings, 0, sizeof(g_MouseAxisBindings));

    g_CursorGrabbed = false;
    RegisterKeyControl("grab-cursor", OnCursorGrabEvent, NULL, NULL);

    return true;
}

void DestroyMouseBindings()
{
}

void OnCursorMove( double x, double y )
{
    if(g_CursorGrabbed)
    {
        static double totalX = 0;
        static double totalY = 0;

        totalX += x;
        totalY += y;

        if(g_MouseAxisBindings[MOUSE_AXIS_X].isEnabled)
            HandleAxisEvent(g_MouseAxisBindings[MOUSE_AXIS_X].axisControl, totalX);

        if(g_MouseAxisBindings[MOUSE_AXIS_Y].isEnabled)
            HandleAxisEvent(g_MouseAxisBindings[MOUSE_AXIS_Y].axisControl, totalY);

        glfwSetCursorPos((GLFWwindow*)GetGLFWwindow(), 0, 0);
    }
}

void OnCursorGrabEvent( const char* name, bool pressed, void* context )
{
    if(pressed)
    {
        if(g_CursorGrabbed)
        {
            SetCursorMode(GLFW_CURSOR_NORMAL);
            g_CursorGrabbed = false;
        }
        else
        {
            SetCursorMode(GLFW_CURSOR_DISABLED);
            g_CursorGrabbed = true;
            glfwSetCursorPos((GLFWwindow*)GetGLFWwindow(), 0, 0);
        }
    }
}

void OnMouseButtonAction( int button, bool pressed )
{
    assert(button < MOUSE_BUTTON_COUNT);
    if(g_MouseButtonBindings[button].isEnabled)
        HandleKeyEvent(g_MouseButtonBindings[button].keyControl, pressed);
}

void OnMouseScroll( double xoffset, double yoffset )
{
    static double totalX = 0;
    static double totalY = 0;

    totalX += xoffset;
    totalY += yoffset;

    if(g_MouseAxisBindings[MOUSE_SCROLL_AXIS_X].isEnabled)
        HandleAxisEvent(g_MouseAxisBindings[MOUSE_SCROLL_AXIS_X].axisControl, totalX);

    if(g_MouseAxisBindings[MOUSE_SCROLL_AXIS_Y].isEnabled)
        HandleAxisEvent(g_MouseAxisBindings[MOUSE_SCROLL_AXIS_Y].axisControl, totalY);
}

bool CreateMouseButtonBinding( int button, int keyControl )
{
    MouseButtonBinding binding;
    binding.isEnabled = true;
    binding.keyControl = keyControl;

    g_MouseButtonBindings[button] = binding;

    return true;
}

bool Mouse_CreateKeyBindingFromString( const char* str, int keyControl )
{
#define MOUSE_BUTTON(N,B) if(strcmp(str, (N)) == 0) return CreateMouseButtonBinding((B), keyControl);
    MOUSE_BUTTON("mouse-button:0", 0)
    MOUSE_BUTTON("mouse-button:1", 1)
    MOUSE_BUTTON("mouse-button:2", 2)
    MOUSE_BUTTON("mouse-button:3", 3)
    MOUSE_BUTTON("mouse-button:4", 4)
    MOUSE_BUTTON("mouse-button:5", 5)
    MOUSE_BUTTON("mouse-button:6", 6)
    MOUSE_BUTTON("mouse-button:7", 7)
    // Everything beyond is clearly insane. : )
#undef MOUSE_BUTTON

    return false;
}

bool CreateMouseAxisBinding( int axis, int axisControl )
{
    MouseAxisBinding binding;
    binding.isEnabled = true;
    binding.axisControl = axisControl;

    g_MouseAxisBindings[axis] = binding;

    return true;
}

bool Mouse_CreateAxisBindingFromString( const char* str, int axisControl )
{
    // mouse:0
    // mouse:1
    // mouse-scroll:0
    // mouse-scroll:1

#define MOUSE_AXIS(N,A) if(strcmp(str, (N)) == 0) return CreateMouseAxisBinding((A), axisControl);
    MOUSE_AXIS("mouse:0",0)
    MOUSE_AXIS("mouse:1",1)
    MOUSE_AXIS("mouse-scroll:0",2)
    MOUSE_AXIS("mouse-scroll:1",3)
#undef MOUSE_AXIS

    return false;
}
