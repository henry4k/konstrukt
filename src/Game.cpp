#include "Math.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Config.h"
#include "Window.h"
#include "OpenGL.h"
#include "Background.h"
#include "Game.h"

void OnFramebufferResize( int width, int height );

bool InitGame( const int argc, char** argv )
{
    if(!InitConfig(argc, argv))
        return false;

    if(!InitWindow())
        return false;

    if(!InitBackground())
        return false;

    SetFrambufferFn(OnFramebufferResize);

    return true;
}

void DestroyGame()
{
    DestroyBackground();
    DestroyWindow();
    DestroyConfig();
}

void RunGame()
{
    using namespace glm;

    glClearColor(0.5, 0.5, 0.5, 1);

    //double lastTime = glfwGetTime();
    while(!WindowShouldClose())
    {
        glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);

        const mat4 lookAtMatrix = lookAt(
            vec3(4,4,4),
            vec3(0,0,0),
            vec3(0,1,0)
        );
        glLoadMatrixf(value_ptr(lookAtMatrix));

        glRotatef((float) glfwGetTime() * 50.f, 0.f, 0.f, 1.f);
        glBegin(GL_TRIANGLES);
            glColor3f(1.f, 0.f, 0.f); glVertex3f(-0.6f, -0.4f, 0.f);
            glColor3f(0.f, 1.f, 0.f); glVertex3f( 0.6f, -0.4f, 0.f);
            glColor3f(0.f, 0.f, 1.f); glVertex3f(  0.f,  0.6f, 0.f);
        glEnd();

        /*
        // Simulation
        const double curTime = glfwGetTime();
        // TODO: Simulate stuff here!
        lastTime = curTime;

        // Render world
        glClear(GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        // TODO: Render world here!

        // Render HUD
        glClear(GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        // TODO: Render HUD here!
        */

        SwapBuffers();
    }
}

void OnFramebufferResize( int width, int height )
{
    using namespace glm;

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    const float aspect = float(width) / float(height);
    const mat4 perspectivicMatrix = perspective(90.0f, aspect, 0.1f, 100.0f);
    glLoadMatrixf(value_ptr(perspectivicMatrix));

    glMatrixMode(GL_MODELVIEW);
}
