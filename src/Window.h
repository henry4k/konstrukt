#ifndef __WINDOW__
#define __WINDOW__

void InitWindow( const char* name, int width, int height );
void DeinitWindow();
void SwapBuffers();
bool WindowShouldClose();

#endif
