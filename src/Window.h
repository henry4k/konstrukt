#ifndef __WINDOW__
#define __WINDOW__

namespace Window
{
    void Init( const char* name, int width, int height );
    void Deinit();
    void SwapBuffers();
    bool ShouldClose();
}

#endif
