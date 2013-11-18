#ifndef __MOUSE_BINDINGS__
#define __MOUSE_BINDINGS__

bool InitMouseBindings();
void DestroyMouseBindings();
bool Mouse_CreateKeyBindingFromString( const char* str, int axisControl );
bool Mouse_CreateAxisBindingFromString( const char* str, int axisControl );

#endif
