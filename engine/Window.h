#ifndef __KONSTRUKT_WINDOW__
#define __KONSTRUKT_WINDOW__

// Callback functions
typedef void (*FramebufferResizeFn)( int width, int height );
typedef void (*CursorMoveFn)( double x, double y );
typedef void (*MouseButtonActionFn)( int button, bool pressed );
typedef void (*MouseScrollFn)( double xoffset, double yoffset );
typedef void (*KeyActionFn)( int key, int scancode, bool pressed );

// General
bool InitWindow();
void DestroyWindow();
void FlagWindowForClose();
bool WindowShouldClose();
void SwapBuffers();
void* GetGLFWwindow();
void GetFramebufferSize( int* width, int* height );

// Input
void SetCursorMode( int mode );

// Callbacks setter
void SetWindowTitle( const char* title );
void SetFrambufferFn( FramebufferResizeFn fn );
void SetCursorMoveFn( CursorMoveFn fn );
void SetMouseButtonActionFn( MouseButtonActionFn fn );
void SetMouseScrollFn( MouseScrollFn fn );
void SetKeyActionFn( KeyActionFn fn );

#endif
