#include <string.h>
#include <vector>
#include <map>

#include "Common.h"
#include "Config.h"
#include "OpenGL.h"
#include "Window.h"
#include "Lua.h"
#include "Controls.h"
#include "ControlBindings/KeyboardBindings.h"
#include "ControlBindings/MouseBindings.h"
#include "ControlBindings/JoystickBindings.h"


static const int MAX_CONTROL_NAME_LENGTH = 31;

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
    void* context;
    bool* value;
};

struct AxisControl
{
    char name[MAX_CONTROL_NAME_LENGTH+1];
    AxisControlActionFn callback;
    void* context;
    float* value;
    float lastValue; // Needed to generate deltas
    float currentAcceleration; // Used when interpretation mode is acceleration.
    AxisInterpretation interpretation;
    float sensitivity; // Raw input is multiplied by this factor.
};

std::vector<KeyControl>  g_KeyControls;
std::vector<AxisControl> g_AxisControls;

bool InitControls()
{
    g_KeyControls.clear();
    g_AxisControls.clear();

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
    Joystick_UpdateControls(timeFrame);

    for(int i = 0; i < g_AxisControls.size(); ++i)
    {
        AxisControl* axisControl = &g_AxisControls[i];

        if(axisControl->interpretation == AXIS_ACCELERATION_VALUE)
        {
            const float delta = axisControl->currentAcceleration * timeFrame;
            axisControl->lastValue += delta;

            if(axisControl->value)
                *axisControl->value = axisControl->lastValue;

            if(axisControl->callback)
                axisControl->callback(axisControl->name, axisControl->lastValue, delta, axisControl->context);
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

bool RegisterKeyControl( const char* name, KeyControlActionFn callback, void* context, bool* value )
{
    if(!ControlNameIsOkay(name))
        return false;

    KeyControl keyControl;
    strncpy(keyControl.name, name, MAX_CONTROL_NAME_LENGTH);
    keyControl.name[MAX_CONTROL_NAME_LENGTH] = '\0';
    keyControl.callback = callback;
    keyControl.context = context;
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

bool RegisterAxisControl( const char* name, AxisControlActionFn callback, void* context, float* value )
{
    if(!ControlNameIsOkay(name))
        return false;

    AxisControl axisControl;
    strncpy(axisControl.name, name, MAX_CONTROL_NAME_LENGTH);
    axisControl.name[MAX_CONTROL_NAME_LENGTH] = '\0';
    axisControl.callback = callback;
    axisControl.context = context;
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
        keyControl->callback(keyControl->name, pressed, keyControl->context);
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
                axisControl->callback(axisControl->name, value, delta, axisControl->context);
            }

            axisControl->lastValue = value;

        case AXIS_ACCELERATION_VALUE:
            axisControl->currentAcceleration = value;
            // See UpdateControls()
    }
}

bool CreateKeyBindingFromString( const char* str, int keyControl )
{
    if(Keyboard_CreateKeyBindingFromString(str, keyControl))
        return true;

    if(Mouse_CreateKeyBindingFromString(str, keyControl))
        return true;

    if(Joystick_CreateKeyBindingFromString(str, keyControl))
        return true;

    return false;
}

bool CreateAxisBindingFromString( const char* str, int axisControl )
{
    if(Mouse_CreateAxisBindingFromString(str, axisControl))
        return true;

    if(Joystick_CreateAxisBindingFromString(str, axisControl))
        return true;

    return false;
}


// --- lua bindings ---

const char* KEY_CONTROL_ACTION_EVENT_NAME = "KeyControlAction";
const char* AXIS_CONTROL_ACTION_EVENT_NAME = "AxisControlAction";

int g_KeyControlActionEvent = LUA_INVALID_EVENT;
int g_AxisControlActionEvent = LUA_INVALID_EVENT;

void OnLuaKeyControlAction( const char* name, bool pressed, void* context )
{
    lua_State* l = GetLuaState();
    lua_pushstring(l, name);
    lua_pushboolean(l, pressed);
    FireLuaEvent(l, g_KeyControlActionEvent, 2, false);
}

int Lua_RegisterKeyControl( lua_State* l )
{
    const char* name = luaL_checkstring(l, 1);

    const bool success = RegisterKeyControl(name, OnLuaKeyControlAction, NULL, NULL);
    lua_pushboolean(l, success);
    return 1;
}

void OnLuaAxisControlAction( const char* name, float absolute, float delta, void* context)
{
    lua_State* l = GetLuaState();
    lua_pushstring(l, name);
    lua_pushnumber(l, absolute);
    lua_pushnumber(l, delta);
    FireLuaEvent(l, g_AxisControlActionEvent, 3, false);
}

int Lua_RegisterAxisControl( lua_State* l )
{
    const char* name = luaL_checkstring(l, 1);

    const bool success = RegisterAxisControl(name, OnLuaAxisControlAction, NULL, NULL);
    lua_pushboolean(l, success);
    return 1;
}

AutoRegisterInLua()
{
    g_KeyControlActionEvent = RegisterLuaEvent(KEY_CONTROL_ACTION_EVENT_NAME);
    if(g_KeyControlActionEvent == LUA_INVALID_EVENT)
        return false;

    g_AxisControlActionEvent = RegisterLuaEvent(AXIS_CONTROL_ACTION_EVENT_NAME);
    if(g_AxisControlActionEvent == LUA_INVALID_EVENT)
        return false;

    return
        RegisterFunctionInLua("RegisterKeyControl", Lua_RegisterKeyControl) &&
        RegisterFunctionInLua("RegisterAxisControl", Lua_RegisterAxisControl);
}
