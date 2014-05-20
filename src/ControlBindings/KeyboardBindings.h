#ifndef __APOAPSIS_KEYBOARD_BINDINGS__
#define __APOAPSIS_KEYBOARD_BINDINGS__

bool InitKeyboardBindings();
void DestroyKeyboardBindings();
bool Keyboard_CreateKeyBindingFromString( const char* str, int keyControl );

#endif
