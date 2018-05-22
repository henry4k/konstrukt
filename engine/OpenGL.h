#ifndef __KONSTRUKT_OPENGL__
#define __KONSTRUKT_OPENGL__

// Prevent glad.h from including windows.h:
#include "Constants.h" // OS_CALLING_CONVENTION
#if defined(OS_CALLING_CONVENTION)
    #define APIENTRY OS_CALLING_CONVENTION
#endif

#include <glad.h>

// Sanity check:
#if defined(_WINDOWS_)
    #error windows.h should not be included
#endif

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#endif
