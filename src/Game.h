#ifndef __GAME__
#define __GAME__

namespace Game
{
    void OnWindowResize( int width, int height );
    void OnMouseButtonAction( int button, bool pressed );
    void OnMouseScroll( double xoffset, double yoffset );
    void OnMouseMove( double x, double y, double xoffset, double yoffset );
    void OnKeyAction( int key, bool pressed );
    int Loop();
}

#endif
