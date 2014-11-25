#include "OpenGL.h" // glfwGetTime
#include "Time.h"


bool InitTime()
{
    return true;
}

void DestroyTime()
{
}

double GetTime()
{
    return glfwGetTime();
}
