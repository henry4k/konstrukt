#ifndef __APOAPSIS_CONTROL_BINDINGS_MOUSE__
#define __APOAPSIS_CONTROL_BINDINGS_MOUSE__

bool InitMouseBindings();
void DestroyMouseBindings();
bool CreateMouseBindingFromString( const char* str, int control );

#endif
