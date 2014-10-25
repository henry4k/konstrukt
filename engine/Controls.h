#ifndef __APOAPSIS_CONTROLS__
#define __APOAPSIS_CONTROLS__

// TODO: Nur delta uebermitteln?
typedef void (*ControlActionFn)( const char* name, float absolute, float delta, void* context );

bool InitControls();
void DestroyControls();
void UpdateControls( float timeFrame );

/**
 * Controls have an value range from -1 to +1.
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
 * @return
 *      `false` if there is already a control with the same name or if no
 *      binding was configured for the control.
 */
bool RegisterControl( const char* name, ControlActionFn callback, void* context );

// Internal: (needed by the binding implementations)
void HandleControlEvent( int controlIndex, float value );

#endif
