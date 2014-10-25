#ifndef __APOAPSIS_CONTROL_BINDINGS_KEYBOARD__
#define __APOAPSIS_CONTROL_BINDINGS_KEYBOARD__

bool InitKeyboardBindings();
void DestroyKeyboardBindings();
bool CreateKeyboardBindingFromString( const char* str, int control );

#endif
