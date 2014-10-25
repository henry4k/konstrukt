#include <string.h>
#include <map>
#include "../Common.h"
#include "../OpenGL.h"
#include "../Window.h"
#include "../Controls.h"
#include "Keyboard.h"

static void OnKeyAction( int key, int keycode, bool pressed );
bool CreateKeyboardBindingFromString( const char* str, int control );

struct KeyboardBinding
{
    int control;
    // TODO: mod-keys?
    // keycode: see OnKeyAction
};

static std::map<int,KeyboardBinding> g_KeyboardBindings;

bool InitKeyboardBindings()
{
    SetKeyActionFn(OnKeyAction);
    g_KeyboardBindings.clear();
    return true;
}

void DestroyKeyboardBindings()
{
}

static void OnKeyAction( int key, int keycode, bool pressed )
{
    // scancodes would be better, but how can we convert config strings to scancodes?
    const std::map<int,KeyboardBinding>::const_iterator i = g_KeyboardBindings.find(key);
    if(i != g_KeyboardBindings.end())
        HandleControlEvent(i->second.control, pressed);
}

static bool CreateKeyboardKeycodeBinding( int keycode, int control )
{
    KeyboardBinding binding;
    binding.control = control;

    g_KeyboardBindings[keycode] = binding;

    return true;
}

static char Uppercase( char ch )
{
    static const char offset = 'A'-'a';

    if(ch >= 'a' && ch <= 'z')
        return ch + offset;
    else
        return ch;
}

bool CreateKeyboardBindingFromString( const char* str, int control )
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
        return CreateKeyboardKeycodeBinding(Uppercase(str[0]), control);

#define KEYCODE(N,C) if(strcmp(str, (N)) == 0) return CreateKeyboardKeycodeBinding((C), control);
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

    return false;
}
