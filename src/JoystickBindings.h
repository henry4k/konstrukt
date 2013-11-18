#ifndef __JOYSTICK_BINDINGS__
#define __JOYSTICK_BINDINGS__

bool InitJoystickBindings();
void DestroyJoystickBindings();
void Joystick_UpdateControls( float timeFrame );
bool Joystick_CreateKeyBindingFromString( const char* str, int keyControl );
bool Joystick_CreateAxisBindingFromString( const char* str, int axisControl );

#endif
