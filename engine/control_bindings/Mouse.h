#ifndef __KONSTRUKT_CONTROL_BINDINGS_MOUSE__
#define __KONSTRUKT_CONTROL_BINDINGS_MOUSE__

bool InitMouseBindings();
void DestroyMouseBindings();
bool CreateMouseBindingFromString( const char* str, int control );

#endif
