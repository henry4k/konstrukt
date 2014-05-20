#ifndef __APOAPSIS_CONTROL_BINDINGS_KEYBOARD__
#define __APOAPSIS_CONTROL_BINDINGS_KEYBOARD__

bool InitKeyboardBindings();
void DestroyKeyboardBindings();
bool Keyboard_CreateKeyBindingFromString( const char* str, int keyControl );

#endif
