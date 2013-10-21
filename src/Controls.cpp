#include <string.h>
#include <vector>
#include <map>
#include "Common.h"
#include "Config.h"
#include "OpenGL.h"
#include "Window.h"
#include "Controls.h"

static const int MAX_CONTROL_NAME_LENGTH = 31;

void OnCursorMove( double x, double y );
void OnMouseButtonAction( int button, bool pressed );
void OnMouseScroll( double xoffset, double yoffset );
void OnKeyAction( int key, int keycode, bool pressed );

void OnCursorGrabEvent( const char* name, bool pressed );
bool CreateKeyBindingFromString( const char* str, int keyControl );
bool CreateAxisBindingFromString( const char* str, int axisControl );

enum AxisInterpretation
{
    AXIS_ABSOLUTE_VALUE,
    AXIS_ACCELERATION_VALUE
};

struct KeyControl
{
    char name[MAX_CONTROL_NAME_LENGTH+1];
    KeyControlActionFn callback;
    bool* value;
};

struct AxisControl
{
    char name[MAX_CONTROL_NAME_LENGTH+1];
    AxisControlActionFn callback;
    float* value;
    float lastValue; // Needed to generate deltas
    float currentAcceleration; // Used when interpretation mode is acceleration.
    AxisInterpretation interpretation;
    float sensitivity; // Raw input is multiplied by this factor.
};

struct KeyBinding
{
    int keyControl;
    // TODO: mod-keys?
    // keycode: see OnKeyAction
};

struct MouseButtonBinding
{
    bool isEnabled;
    int keyControl;
    // button: see OnMouseButtonAction
};

/*
struct MouseScrollBinding // TODO: Maybe this should be also an axis.
{
    bool isEnabled;
    int keyControl;
    // direction: see OnMouseScrollAction
};
*/

struct MouseAxisBinding
{
    bool isEnabled;
    int axisControl;
    // axis: see OnCursorMove
};

/* TODO:

struct JoystickButtonBinding
{
    int control;
    int joystick;
    int axis;
};

struct JoystickAxisBinding
{
    int control;
    int joystick;
    int axis;
};
*/

static const int MOUSE_BUTTON_COUNT = GLFW_MOUSE_BUTTON_LAST+1;

std::vector<KeyControl>  g_KeyControls;
std::vector<AxisControl> g_AxisControls;

std::map<int,KeyBinding> g_KeyBindings;
MouseButtonBinding       g_MouseButtonBindings[MOUSE_BUTTON_COUNT];
//MouseScrollBinding     g_MouseScrollBindings[4]; // TODO: Enum for this please. (+x, -x, +y, -y)
MouseAxisBinding         g_MouseAxisBindings[2];

bool g_CursorGrabbed;


bool InitControls()
{
    SetCursorMoveFn(OnCursorMove);
    SetMouseButtonActionFn(OnMouseButtonAction);
    SetMouseScrollFn(OnMouseScroll);
    SetKeyActionFn(OnKeyAction);

    g_KeyControls.clear();
    g_AxisControls.clear();

    g_KeyBindings.clear();
    memset(g_MouseButtonBindings, 0, sizeof(g_MouseButtonBindings));
    //memset(g_MouseScrollBindings, 0, sizeof(g_MouseScrollBindings));
    memset(g_MouseAxisBindings, 0, sizeof(g_MouseAxisBindings));

    g_CursorGrabbed = false;
    RegisterKeyControl("grab-cursor", OnCursorGrabEvent, NULL);

    return true;
}

void DestroyControls()
{
}

void UpdateControls( float timeDelta )
{
    for(int i = 0; i < g_AxisControls.size(); ++i)
    {
        AxisControl* axisControl = &g_AxisControls[i];

        if(axisControl->interpretation == AXIS_ACCELERATION_VALUE)
        {
            const float delta = axisControl->currentAcceleration * timeDelta;
            axisControl->lastValue += delta;

            if(axisControl->value)
                *axisControl->value = axisControl->lastValue;

            if(axisControl->callback)
                axisControl->callback(axisControl->name, axisControl->lastValue, delta);
        }
    }
}

bool ControlNameIsOkay( const char* name )
{
    if(strlen(name) > MAX_CONTROL_NAME_LENGTH)
    {
        Error("Name of control '%s' is too long.", name);
        return false;
    }

    for(int i = 0; i < g_KeyControls.size(); ++i)
    {
        if(strcmp(name, g_KeyControls[i].name) == 0)
        {
            Error("Control '%s' is not unique.", name);
            return false;
        }
    }

    for(int i = 0; i < g_AxisControls.size(); ++i)
    {
        if(strcmp(name, g_AxisControls[i].name) == 0)
        {
            Error("Control '%s' is not unique.", name);
            return false;
        }
    }

    return true;
}

bool RegisterKeyControl( const char* name, KeyControlActionFn callback, bool* value )
{
    if(!ControlNameIsOkay(name))
        return false;

    KeyControl keyControl;
    strncpy(keyControl.name, name, MAX_CONTROL_NAME_LENGTH);
    keyControl.name[MAX_CONTROL_NAME_LENGTH] = '\0';
    keyControl.callback = callback;
    keyControl.value = value;

    const int keyControlIndex = g_KeyControls.size();
    g_KeyControls.push_back(keyControl);

    const char* bindingName = GetConfigString(Format("control.%s", name), NULL);
    if(bindingName)
    {
        if(CreateKeyBindingFromString(bindingName, keyControlIndex))
        {
            Log("Key control '%s' is bound to '%s'.", name, bindingName);
        }
        else
        {
            Error("Failed to create key binding for '%s'.", bindingName);
            return false;
        }
    }
    else
    {
        Log("Key control '%s' is unused.", name);
        return false;
    }

    return true;
}

bool RegisterAxisControl( const char* name, AxisControlActionFn callback, float* value )
{
    if(!ControlNameIsOkay(name))
        return false;

    AxisControl axisControl;
    strncpy(axisControl.name, name, MAX_CONTROL_NAME_LENGTH);
    axisControl.name[MAX_CONTROL_NAME_LENGTH] = '\0';
    axisControl.callback = callback;
    axisControl.value = value;
    axisControl.lastValue = 0;
    axisControl.currentAcceleration = 0;
    axisControl.sensitivity = GetConfigFloat(Format("control.%s-sensitivity", name), 1);
    axisControl.interpretation = AXIS_ABSOLUTE_VALUE;

    const char* axisInterpretation = GetConfigString(Format("control.%s-interpretation", name), NULL);
    if(axisInterpretation)
    {
        if(strcmp(axisInterpretation, "absolute") == 0)
        {
            axisControl.interpretation = AXIS_ABSOLUTE_VALUE;
        }
        else if(strcmp(axisInterpretation, "acceleration") == 0)
        {
            axisControl.interpretation = AXIS_ACCELERATION_VALUE;
        }
        else
        {
            Error("Unknown axis interpretation mode '%s' of control '%s'.", axisInterpretation, name);
            return false;
        }
    }

    const int axisControlIndex = g_AxisControls.size();
    g_AxisControls.push_back(axisControl);

    const char* bindingName = GetConfigString(Format("control.%s", name), NULL);
    if(bindingName)
    {
        if(CreateAxisBindingFromString(bindingName, axisControlIndex))
        {
            Log("Axis control '%s' is bound to '%s'.", name, bindingName);
        }
        else
        {
            Error("Failed to create axis binding for '%s'.", bindingName);
            return false;
        }
    }
    else
    {
        Log("Axis control '%s' is unused.", name);
        return false;
    }

    return true;
}

void HandleKeyEvent( int keyControlIndex, bool pressed )
{
    assert(keyControlIndex < g_KeyControls.size());
    const KeyControl* keyControl = &g_KeyControls[keyControlIndex];

    if(keyControl->value)
        *keyControl->value = pressed;

    if(keyControl->callback)
        keyControl->callback(keyControl->name, pressed);
}

void HandleAxisEvent( int axisControlIndex, float value )
{
    assert(axisControlIndex < g_AxisControls.size());
    AxisControl* axisControl = &g_AxisControls[axisControlIndex];

    value *= axisControl->sensitivity;

    switch(axisControl->interpretation)
    {
        case AXIS_ABSOLUTE_VALUE:
            if(axisControl->value)
                *axisControl->value = value;

            if(axisControl->callback)
            {
                const float delta = value - axisControl->lastValue;
                axisControl->callback(axisControl->name, value, delta);
            }

            axisControl->lastValue = value;

        case AXIS_ACCELERATION_VALUE:
            axisControl->currentAcceleration = value;
            // See UpdateControls()
    }
}

void OnCursorMove( double x, double y )
{
    if(g_CursorGrabbed)
    {
        static double totalX = 0;
        static double totalY = 0;

        totalX += x;
        totalY += y;

        if(g_MouseAxisBindings[0].isEnabled)
            HandleAxisEvent(g_MouseAxisBindings[0].axisControl, totalX);

        if(g_MouseAxisBindings[1].isEnabled)
            HandleAxisEvent(g_MouseAxisBindings[1].axisControl, totalY);

        glfwSetCursorPos((GLFWwindow*)GetGLFWwindow(), 0, 0);
    }
}

void OnCursorGrabEvent( const char* name, bool pressed )
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
    Log("scroll: %f %f", xoffset, yoffset);
}

void OnKeyAction( int key, int keycode, bool pressed )
{
    // scancodes would be better, but how can we convert config strings to scancodes?
    const std::map<int,KeyBinding>::const_iterator i = g_KeyBindings.find(key);
    if(i != g_KeyBindings.end())
        HandleKeyEvent(i->second.keyControl, pressed);
}

bool CreateKeycodeBinding( int keycode, int keyControl )
{
    KeyBinding binding;
    binding.keyControl = keyControl;

    g_KeyBindings[keycode] = binding;

    return true;
}

bool CreateMouseButtonBinding( int button, int keyControl )
{
    MouseButtonBinding binding;
    binding.isEnabled = true;
    binding.keyControl = keyControl;

    g_MouseButtonBindings[button] = binding;

    return true;
}

char Uppercase( char ch )
{
    static const char offset = 'A'-'a';

    if(ch >= 'a' && ch <= 'z')
        return ch + offset;
    else
        return ch;
}

bool CreateKeyBindingFromString( const char* str, int keyControl )
{
    // a b c 1 2 3
    // escape
    // space
    // return
    // lctrl
    // rshift
    // ctrl+a
    // shift+ctrl+a

    const int length = strlen(str);

    if(length == 1)
        return CreateKeycodeBinding(Uppercase(str[0]), keyControl);

#define KEYCODE(N,C) if(strcmp(str, (N)) == 0) return CreateKeycodeBinding((C), keyControl);
    KEYCODE("space", GLFW_KEY_SPACE)
    KEYCODE("escape", GLFW_KEY_ESCAPE)
    KEYCODE("enter", GLFW_KEY_ENTER)
    KEYCODE("tab", GLFW_KEY_TAB)
    KEYCODE("backspace", GLFW_KEY_BACKSPACE)
    KEYCODE("insert", GLFW_KEY_INSERT)
    KEYCODE("delete", GLFW_KEY_DELETE)
    KEYCODE("right", GLFW_KEY_RIGHT)
    KEYCODE("left", GLFW_KEY_LEFT)
    KEYCODE("up", GLFW_KEY_UP)
    KEYCODE("down", GLFW_KEY_DOWN)
    // and so on
    // TODO: Finish this damn list.
    /*
    KEYCODE("apostrophe", GLFW_KEY_APOSTROPHE)
    KEYCODE("comma", GLFW_KEY_COMMA)
    KEYCODE("minus", GLFW_KEY_MINUS)
    KEYCODE("period", GLFW_KEY_PERIOD)
    KEYCODE("slash", GLFW_KEY_SLASH)
    KEYCODE("semicolon", GLFW_KEY_SEMICOLON)
    KEYCODE("equal", GLFW_KEY_EQUAL)
    */
#undef KEYCODE

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

bool CreateAxisBindingFromString( const char* str, int axisControl )
{
    // mouse:0
    // mouse:1
    // mouse-scroll:0
    // mouse-scroll:1
    // xpad:0
    // xpad:1

#define MOUSE_AXIS(N,A) if(strcmp(str, (N)) == 0) return CreateMouseAxisBinding((A), axisControl);
    MOUSE_AXIS("mouse:0",0)
    MOUSE_AXIS("mouse:1",1)
#undef MOUSE_AXIS

    return false;
}
