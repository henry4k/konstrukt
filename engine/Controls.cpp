#include <assert.h>
#include <string.h>
#include <vector>

#include "Common.h"
#include "Config.h"
#include "OpenGL.h"
#include "Window.h"
#include "Controls.h"
#include "control_bindings/Keyboard.h"
#include "control_bindings/Mouse.h"
#include "control_bindings/Joystick.h"


static const int MAX_CONTROL_NAME_SIZE = 32;

static bool CreateControlBindingFromString( const char* str, int control );

enum Interpretation
{
    ABSOLUTE_VALUE,
    ACCELERATION_VALUE
};

struct Control
{
    char name[MAX_CONTROL_NAME_SIZE];
    ControlActionFn callback;
    void* context;

    float lastValue; // Needed to generate deltas
    float currentAcceleration; // Used when interpretation mode is acceleration.
    Interpretation interpretation;
    float sensitivity; // Raw input is multiplied by this factor.
};

static std::vector<Control> g_Controls;

bool InitControls()
{
    g_Controls.clear();

    return
        InitKeyboardBindings() &&
        InitMouseBindings() &&
        InitJoystickBindings();
}

void DestroyControls()
{
    DestroyKeyboardBindings();
    DestroyMouseBindings();
    DestroyJoystickBindings();
}

void UpdateControls( float timeFrame )
{
    UpdateJoystickControls(timeFrame);

    for(size_t i = 0; i < g_Controls.size(); ++i)
    {
        Control* control = &g_Controls[i];

        if(control->interpretation == ACCELERATION_VALUE)
        {
            const float delta = control->currentAcceleration * timeFrame;
            control->lastValue += delta;

            if(control->callback)
                control->callback(control->name, control->lastValue, delta, control->context);
        }
    }
}

static bool ControlNameIsOkay( const char* name )
{
    if(strlen(name) > MAX_CONTROL_NAME_SIZE-1)
    {
        Error("Name of control '%s' is too long.", name);
        return false;
    }

    for(size_t i = 0; i < g_Controls.size(); ++i)
    {
        if(strcmp(name, g_Controls[i].name) == 0)
        {
            Error("Control '%s' is not unique.", name);
            return false;
        }
    }

    return true;
}

bool RegisterControl( const char* name, ControlActionFn callback, void* context )
{
    if(!ControlNameIsOkay(name))
        return false;

    Control control;
    CopyString(name, control.name, sizeof(control.name));
    control.callback = callback;
    control.context = context;
    control.lastValue = 0;
    control.currentAcceleration = 0;
    control.sensitivity = GetConfigFloat(Format("control.%s-sensitivity", name), 1);
    control.interpretation = ABSOLUTE_VALUE;

    const char* interpretation = GetConfigString(Format("control.%s-interpretation", name), NULL);
    if(interpretation)
    {
        if(strcmp(interpretation, "absolute") == 0)
        {
            control.interpretation = ABSOLUTE_VALUE;
        }
        else if(strcmp(interpretation, "acceleration") == 0)
        {
            control.interpretation = ACCELERATION_VALUE;
        }
        else
        {
            Error("Unknown interpretation mode '%s' of control '%s'.", interpretation, name);
            return false;
        }
    }

    const int controlIndex = g_Controls.size();
    g_Controls.push_back(control);

    const char* bindingName = GetConfigString(Format("control.%s", name), NULL);
    if(bindingName)
    {
        if(CreateControlBindingFromString(bindingName, controlIndex))
        {
            Log("Control '%s' is bound to '%s'.", name, bindingName);
        }
        else
        {
            Error("Failed to create binding for '%s'.", bindingName);
            return false;
        }
    }
    else
    {
        Error("Control '%s' is unused.", name);
        return false;
    }

    return true;
}

void HandleControlEvent( int controlIndex, float value )
{
    assert(controlIndex < (int)g_Controls.size());
    Control* control = &g_Controls[controlIndex];

    value *= control->sensitivity;

    switch(control->interpretation)
    {
        case ABSOLUTE_VALUE:
            if(control->callback)
            {
                const float delta = value - control->lastValue;
                control->callback(control->name, value, delta, control->context);
            }

            control->lastValue = value;

        case ACCELERATION_VALUE:
            control->currentAcceleration = value;
            // See UpdateControls()
    }
}

static bool CreateControlBindingFromString( const char* str, int control )
{
    return CreateKeyboardBindingFromString(str, control) ||
           CreateMouseBindingFromString(str, control) ||
           CreateJoystickBindingFromString(str, control);
}
