#include <string.h>
#include <stdio.h> // sscanf
#include "../Warnings.h"
#include "../Common.h"
#include "../Config.h"
#include "../OpenGL.h"
#include "../Window.h"
#include "../Controls.h"
#include "Joystick.h"

struct JoystickControlBinding
{
    bool enabled;
    int control;
    bool lastValue;
};

struct JoystickData
{
    int buttonCount;
    JoystickControlBinding* buttons;

    int axisCount;
    JoystickControlBinding* axes;
};

static int g_JoystickCount = 0;
static JoystickData* g_Joysticks = NULL;


static void PrintJoysticks()
{
    LogDebug("Joysticks:");
    for(int i = 0; true; ++i)
    {
        const char* name = glfwGetJoystickName(i);
        if(!name)
            break;

        int buttonCount = 0;
        int axisCount   = 0;

        BEGIN_EXTERNAL_CODE
        {
            const unsigned char* buttons = glfwGetJoystickButtons(i, &buttonCount);
            const float* axes = glfwGetJoystickAxes(i, &axisCount);
        }
        END_EXTERNAL_CODE

        LogDebug("  Joystick %d '%s' with %d buttons and %d axes.",
            i,
            name,
            buttonCount,
            axisCount
        );
    }
}

bool InitJoystickBindings()
{
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
        int axisCount   = 0;

        BEGIN_EXTERNAL_CODE
        {
            const unsigned char* buttons = glfwGetJoystickButtons(i, &buttonCount);
            const float* axes = glfwGetJoystickAxes(i, &axisCount);
        }
        END_EXTERNAL_CODE


        data->buttonCount = buttonCount;
        data->buttons = new JoystickControlBinding[buttonCount];
        memset(data->buttons, 0, sizeof(JoystickControlBinding)*buttonCount);

        data->axisCount = axisCount;
        data->axes = new JoystickControlBinding[axisCount];
        memset(data->axes, 0, sizeof(JoystickControlBinding)*axisCount);
    }

    return true;
}
END_EXTERNAL_CODE

void DestroyJoystickBindings()
{
    if(g_Joysticks)
    {
        for(int i=0; i < g_JoystickCount; i++)
        {
            delete[] g_Joysticks[i].buttons;
            delete[] g_Joysticks[i].axes;
        }
        delete[] g_Joysticks;
        g_Joysticks = NULL;
    }
}

static inline int Min( int a, int b )
{
    return (a < b) ? a : b;
}

void UpdateJoystickControls( float timeFrame )
{
    for(int i = 0; i < g_JoystickCount; ++i)
    {
        JoystickData* data = &g_Joysticks[i];

        int buttonCount = 0;
        const unsigned char* buttons = glfwGetJoystickButtons(i, &buttonCount);
        buttonCount = Min(buttonCount, data->buttonCount);
        for(int j = 0; j < buttonCount; ++j)
        {
            JoystickControlBinding* binding = &data->buttons[j];
            if(binding->enabled)
            {
                bool currentValue = (buttons[j] != 0);
                if(currentValue != binding->lastValue)
                {
                    HandleControlEvent(binding->control, currentValue);
                }
                binding->lastValue = currentValue;
            }
        }


        int axisCount = 0;
        const float* axes = glfwGetJoystickAxes(i, &axisCount);
        axisCount = Min(axisCount, data->axisCount);
        for(int j = 0; j < axisCount; ++j)
        {
            JoystickControlBinding* binding = &data->axes[j];
            if(binding->enabled)
            {
                float currentValue = axes[j];
                if(currentValue != binding->lastValue) // TODO: Implement range test here
                {
                    HandleControlEvent(binding->control, currentValue);
                }
                binding->lastValue = currentValue;
            }
        }
    }
}

static bool CreateJoystickButtonControlBinding( int joystick, int button, int keyControl )
{
    if(joystick >= g_JoystickCount)
        return false;

    if(button >= g_Joysticks[joystick].buttonCount)
        return false;

    JoystickControlBinding* binding = &g_Joysticks[joystick].buttons[button];
    binding->enabled = true;
    binding->control = keyControl;

    return true;
}

static bool CreateJoystickAxisControlBinding( int joystick, int axis, int axisControl )
{
    if(joystick >= g_JoystickCount)
        return false;

    if(axis >= g_Joysticks[joystick].axisCount)
        return false;

    JoystickControlBinding* binding = &g_Joysticks[joystick].axes[axis];
    binding->enabled = true;
    binding->control = axisControl;

    return true;
}

bool CreateJoystickBindingFromString( const char* str, int control )
{
    int joystick = 0;
    int button = 0;
    int axis = 0;

    if(sscanf(str, "joystick%d-button:%d", &joystick, &button) == 2)
        return CreateJoystickButtonControlBinding(joystick, button, control);

    if(sscanf(str, "joystick%d-axis:%d", &joystick, &axis) == 2)
        return CreateJoystickAxisControlBinding(joystick, axis, control);

    return false;
}
