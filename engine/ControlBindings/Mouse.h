#ifndef __APOAPSIS_CONTROL_BINDINGS_MOUSE__
#define __APOAPSIS_CONTROL_BINDINGS_MOUSE__

bool InitMouseBindings();
void DestroyMouseBindings();
bool Mouse_CreateKeyBindingFromString( const char* str, int axisControl );
bool Mouse_CreateAxisBindingFromString( const char* str, int axisControl );

#endif
