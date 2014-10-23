#ifndef __APOAPSIS_CONTROL_BINDINGS_JOYSTICK__
#define __APOAPSIS_CONTROL_BINDINGS_JOYSTICK__

bool InitJoystickBindings();
void DestroyJoystickBindings();
void Joystick_UpdateControls( float timeFrame );
bool Joystick_CreateKeyBindingFromString( const char* str, int keyControl );
bool Joystick_CreateAxisBindingFromString( const char* str, int axisControl );

#endif
