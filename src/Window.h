#ifndef __WINDOW__
#define __WINDOW__

// Callback functions
typedef void (*FramebufferResizeFn)( int width, int height );
typedef void (*CursorMoveFn)( double x, double y, double dx, double dy );
typedef void (*MouseButtonActionFn)( int button, bool pressed );
typedef void (*MouseScrollFn)( double xoffset, double yoffset );
typedef void (*KeyActionFn)( int key, int scancode, bool pressed );

// General
bool InitWindow( int width, int height, const char* title );
void FreeWindow();
void FlagWindowForClose();
bool WindowShouldClose();
void SwapBuffers();

// Input
bool IsKeyPressed( int key );
bool IsMouseButtonPressed( int button );
double GetCursorX();
double GetCursorY();
void SetCursorMode( int mode );
void* GetGLFWwindow();

// Callbacks setter
void SetWindowTitle( const char* title );
void SetFrambufferFn( FramebufferResizeFn fn );
void SetCursorMoveFn( CursorMoveFn fn );
void SetMouseButtonActionFn( MouseButtonActionFn fn );
void SetMouseScrollFn( MouseScrollFn fn );
void SetKeyActionFn( KeyActionFn fn );

#endif
