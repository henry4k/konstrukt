#ifndef __APOAPSIS_CONTROLS__
#define __APOAPSIS_CONTROLS__

typedef void (*KeyControlActionFn)( const char* name, bool pressed, void* context );
typedef void (*AxisControlActionFn)( const char* name, float absolute, float delta, void* context );

bool InitControls();
void DestroyControls();
void UpdateControls( float timeFrame );

/**
 * Key controls can either be pressed or released.
 *
 * @param name
 *      A unique identifier for this control.
 *
 * @param callback
 *      Optional function pointer that is called when the keys state changes.
 *
 * @param context
 *      User pointer that is passed to the callback.
 *
 * @param value
 *      Optional value pointer that always holds the state of the key.
 *
 * @return
 *      `false` if there is already a control with the same name.
 */
bool RegisterKeyControl( const char* name, KeyControlActionFn callback, void* context, bool* value );

/**
 * Axis controls have an value range from -1 to +1.
 *
 * @param name
 *      A unique identifier for this control.
 *
 * @param callback
 *      Optional function pointer that is called when the axis state changes.
 *
 * @param context
 *      User pointer that is passed to the callback.
 *
 * @param value
 *      Optional value pointer that always holds the value of the axis.
 *
 * @return
 *      `false` if there is already a control with the same name.
 */
bool RegisterAxisControl( const char* name, AxisControlActionFn callback, void* context, float* value );

// Internal: (needed by the binding implementations)
void HandleKeyEvent( int keyControlIndex, bool pressed );
void HandleAxisEvent( int axisControlIndex, float value );

#endif
