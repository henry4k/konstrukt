#ifndef __GAME__
#define __GAME__

void OnWindowResize( int width, int height );
void OnMouseButtonAction( int button, bool pressed );
void OnMouseScroll( double xoffset, double yoffset );
void OnMouseMove( double x, double y, double xoffset, double yoffset );
void OnKeyAction( int key, bool pressed );
int GameLoop();

#endif
