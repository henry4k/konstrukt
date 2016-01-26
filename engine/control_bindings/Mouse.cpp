#include <assert.h>
#include <string.h>
#include "../Common.h"
#include "../Config.h"
#include "../OpenGL.h"
#include "../Window.h"
#include "../Controls.h"
#include "Mouse.h"

static void OnCursorMove( double x, double y );
static void OnMouseButtonAction( int button, bool pressed );
static void OnMouseScroll( double xoffset, double yoffset );

static void OnCursorGrabEvent( const char* name, float absolute, float delta, void* context );

struct MouseControlBinding
{
    bool isEnabled;
    int control;
    // see OnMouseButtonAction, OnMouseScroll, OnCursorMove
};

static const int MOUSE_BUTTON_COUNT = GLFW_MOUSE_BUTTON_LAST+1;

enum MouseControl
{
    MOUSE_X = 0,
    MOUSE_Y,
    MOUSE_SCROLL_X,
    MOUSE_SCROLL_Y,
    MOUSE_FIRST_BUTTON,
    MOUSE_LAST_BUTTON = MOUSE_FIRST_BUTTON+GLFW_MOUSE_BUTTON_LAST,
    MOUSE_CONTROL_COUNT
};

static MouseControlBinding g_MouseControlBindings[MOUSE_CONTROL_COUNT];

static bool g_CursorGrabbed;


bool InitMouseBindings()
{
    SetCursorMoveFn(OnCursorMove);
    SetMouseButtonActionFn(OnMouseButtonAction);
    SetMouseScrollFn(OnMouseScroll);

    memset(g_MouseControlBindings, 0, sizeof(g_MouseControlBindings));

    g_CursorGrabbed = false;
    RegisterControl("grab-cursor", OnCursorGrabEvent, NULL);

    return true;
}

void DestroyMouseBindings()
{
}

static void OnCursorMove( double x, double y )
{
    if(g_CursorGrabbed)
    {
        static double totalX = 0;
        static double totalY = 0;

        totalX += x;
        totalY += y;

        if(g_MouseControlBindings[MOUSE_X].isEnabled)
            HandleControlEvent(g_MouseControlBindings[MOUSE_X].control, totalX);

        if(g_MouseControlBindings[MOUSE_Y].isEnabled)
            HandleControlEvent(g_MouseControlBindings[MOUSE_Y].control, totalY);

        glfwSetCursorPos((GLFWwindow*)GetGLFWwindow(), 0, 0);
    }
}

static void OnCursorGrabEvent( const char* name, float absolute, float delta, void* context )
{
    const bool pressed = delta > 0;
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

static void OnMouseButtonAction( int button, bool pressed )
{
    int bindingIndex = MOUSE_FIRST_BUTTON+button;
    assert(bindingIndex <= MOUSE_LAST_BUTTON);
    if(g_MouseControlBindings[bindingIndex].isEnabled)
        HandleControlEvent(g_MouseControlBindings[bindingIndex].control, pressed);
}

static void OnMouseScroll( double xoffset, double yoffset )
{
    static double totalX = 0;
    static double totalY = 0;

    totalX += xoffset;
    totalY += yoffset;

    if(g_MouseControlBindings[MOUSE_SCROLL_X].isEnabled)
        HandleControlEvent(g_MouseControlBindings[MOUSE_SCROLL_X].control, totalX);

    if(g_MouseControlBindings[MOUSE_SCROLL_Y].isEnabled)
        HandleControlEvent(g_MouseControlBindings[MOUSE_SCROLL_Y].control, totalY);
}

static bool CreateMouseControlBinding( int bindingIndex, int control )
{
    MouseControlBinding binding;
    binding.isEnabled = true;
    binding.control = control;

    g_MouseControlBindings[bindingIndex] = binding;

    return true;
}

bool CreateMouseBindingFromString( const char* str, int control )
{
#define MOUSE_CONTROL(N,B) if(strcmp(str, (N)) == 0) return CreateMouseControlBinding((B), control);
    MOUSE_CONTROL("mouse:0", MOUSE_X)
    MOUSE_CONTROL("mouse:1", MOUSE_Y)
    MOUSE_CONTROL("mouse-scroll:0", MOUSE_SCROLL_X)
    MOUSE_CONTROL("mouse-scroll:1", MOUSE_SCROLL_Y)

    MOUSE_CONTROL("mouse-button:0", MOUSE_FIRST_BUTTON+0)
    MOUSE_CONTROL("mouse-button:1", MOUSE_FIRST_BUTTON+1)
    MOUSE_CONTROL("mouse-button:2", MOUSE_FIRST_BUTTON+2)
    MOUSE_CONTROL("mouse-button:3", MOUSE_FIRST_BUTTON+3)
    MOUSE_CONTROL("mouse-button:4", MOUSE_FIRST_BUTTON+4)
    MOUSE_CONTROL("mouse-button:5", MOUSE_FIRST_BUTTON+5)
    MOUSE_CONTROL("mouse-button:6", MOUSE_FIRST_BUTTON+6)
    MOUSE_CONTROL("mouse-button:7", MOUSE_FIRST_BUTTON+7)
    // Everything beyond is clearly insane. : )
#undef MOUSE_CONTROL

    return false;
}
