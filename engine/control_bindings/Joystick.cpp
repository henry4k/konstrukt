#include <string.h>
#include "../Common.h"
#include "../Config.h"
#include "../OpenGL.h"
#include "../Window.h"
#include "../Controls.h"
#include "Joystick.h"

struct JoystickButtonBinding
{
    bool enabled;
    int control;
    bool lastValue;
};

struct JoystickAxisBinding
{
    bool enabled;
    int control;
    float lastValue;
};

struct JoystickData
{
    int buttonCount;
    JoystickButtonBinding* buttons;

    int axisCount;
    JoystickAxisBinding* axes;
};

int g_JoystickCount = 0;
JoystickData* g_Joysticks = NULL;


void PrintJoysticks()
{
    Log("Joysticks:");
    for(int i = 0; true; ++i)
    {
        const char* name = glfwGetJoystickName(i);
        if(!name)
            break;

        int buttonCount = 0;
        const unsigned char* buttons = glfwGetJoystickButtons(i, &buttonCount);

        int axisCount = 0;
        const float* axes = glfwGetJoystickAxes(i, &axisCount);

        Log("  Joystick %d '%s' with %d buttons and %d axes.",
            i,
            name,
            buttonCount,
            axisCount
        );
    }
}

bool InitJoystickBindings()
{
    if(GetConfigBool("control.print-joysticks", false))
        PrintJoysticks();

    int joystickCount = 0;
    while(glfwGetJoystickName(joystickCount) != NULL)
        joystickCount++;

    g_JoystickCount = joystickCount;
    g_Joysticks = new JoystickData[joystickCount];
    memset(g_Joysticks, 0, sizeof(JoystickData)*joystickCount);

    for(int i=0; i<joystickCount; ++i)
    {
        JoystickData* data = &g_Joysticks[i];


        int buttonCount = 0;
        const unsigned char* buttons = glfwGetJoystickButtons(i, &buttonCount);

        data->buttonCount = buttonCount;
        data->buttons = new JoystickButtonBinding[buttonCount];
        memset(data->buttons, 0, sizeof(JoystickButtonBinding)*buttonCount);


        int axisCount = 0;
        const float* axes = glfwGetJoystickAxes(i, &axisCount);

        data->axisCount = axisCount;
        data->axes = new JoystickAxisBinding[axisCount];
        memset(data->axes, 0, sizeof(JoystickAxisBinding)*axisCount);
    }

    return true;
}

void DestroyJoystickBindings()
{
    if(g_Joysticks)
        delete[] g_Joysticks;
}

inline int Min( int a, int b )
{
    return (a < b) ? a : b;
}

void Joystick_UpdateControls( float timeFrame )
{
    for(int i = 0; i < g_JoystickCount; ++i)
    {
        JoystickData* data = &g_Joysticks[i];

        int buttonCount = 0;
        const unsigned char* buttons = glfwGetJoystickButtons(i, &buttonCount);
        buttonCount = Min(buttonCount, data->buttonCount);
        for(int j = 0; j < buttonCount; ++j)
        {
            JoystickButtonBinding* binding = &data->buttons[j];
            if(binding->enabled)
            {
                bool currentValue = (buttons[j] != 0);
                if(currentValue != binding->lastValue)
                {
                    HandleKeyEvent(binding->control, currentValue);
                }
                binding->lastValue = currentValue;
            }
        }


        int axisCount = 0;
        const float* axes = glfwGetJoystickAxes(i, &axisCount);
        axisCount = Min(axisCount, data->axisCount);
        for(int j = 0; j < axisCount; ++j)
        {
            JoystickAxisBinding* binding = &data->axes[j];
            if(binding->enabled)
            {
                float currentValue = axes[j];
                if(currentValue != binding->lastValue) // TODO: Implement range test here
                {
                    HandleAxisEvent(binding->control, currentValue);
                }
                binding->lastValue = currentValue;
            }
        }
    }
}

bool CreateJoystickButtonBinding( int joystick, int button, int keyControl )
{
    if(joystick >= g_JoystickCount)
        return false;

    if(button >= g_Joysticks[joystick].buttonCount)
        return false;

    JoystickButtonBinding* binding = &g_Joysticks[joystick].buttons[button];
    binding->enabled = true;
    binding->control = keyControl;

    return true;
}

bool Joystick_CreateKeyBindingFromString( const char* str, int keyControl )
{
    int joystick = 0;
    int button = 0;
    if(sscanf(str, "joystick%d-button:%d", &joystick, &button) != 2)
        return false;
    return CreateJoystickButtonBinding(joystick, button, keyControl);
}

bool CreateJoystickAxisBinding( int joystick, int axis, int axisControl )
{
    if(joystick >= g_JoystickCount)
        return false;

    if(axis >= g_Joysticks[joystick].axisCount)
        return false;

    JoystickAxisBinding* binding = &g_Joysticks[joystick].axes[axis];
    binding->enabled = true;
    binding->control = axisControl;

    return true;
}

bool Joystick_CreateAxisBindingFromString( const char* str, int axisControl )
{
    int joystick = 0;
    int axis = 0;
    if(sscanf(str, "joystick%d-axis:%d", &joystick, &axis) != 2)
        return false;
    return CreateJoystickAxisBinding(joystick, axis, axisControl);
}
