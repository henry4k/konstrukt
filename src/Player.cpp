#include "Common.h"
#include "Math.h"
#include "OpenGL.h"
#include "Controls.h"
#include "Player.h"

//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/quaternion.hpp>

static const float WALK_FORCE = 0.01f;
static const float WALK_DAMPING = 0.99f;

glm::vec2 g_PlayerPosition;
glm::vec2 g_PlayerVelocity;
glm::vec2 g_PlayerRotation;

bool g_ForwardKey;
bool g_BackwardKey;
bool g_RightKey;
bool g_LeftKey;
float g_LookX;
float g_LookY;

bool InitPlayer()
{
    g_PlayerPosition = glm::vec2(0,0);
    g_PlayerVelocity = glm::vec2(0,0);
    g_PlayerRotation = glm::vec2(0,0);

    return
        RegisterKeyControl("forward",  NULL, &g_ForwardKey) &&
        RegisterKeyControl("backward", NULL, &g_BackwardKey) &&
        RegisterKeyControl("right",    NULL, &g_RightKey) &&
        RegisterKeyControl("left",     NULL, &g_LeftKey) &&
        RegisterAxisControl("look-x", NULL, &g_LookX) &&
        RegisterAxisControl("look-y", NULL, &g_LookY);
}

void DestroyPlayer()
{
}

void RotateWorld()
{
    //glLoadMatrixf(glm::value_ptr(g_PlayerRotation.mat4_cast()));
    glRotatef(g_LookX, 0,1,0);
    //glRotatef(g_PlayerRotation.y, 0,0,1);
}

void TranslateWorld()
{
    glTranslatef(g_PlayerPosition.x, 0, g_PlayerPosition.y);
}

void DrawPlayer()
{
}

void UpdatePlayer( float timeDelta )
{
    timeDelta = 1.0f;

    glm::vec2 direction(0,0);

    if(g_ForwardKey)
        direction.y += 1;
    if(g_BackwardKey)
        direction.y -= 1;
    if(g_RightKey)
        direction.x += 1;
    if(g_LeftKey)
        direction.x -= 1;

    glm::vec2 force(0,0);

    g_PlayerVelocity += direction * WALK_FORCE * timeDelta;
    g_PlayerVelocity *= WALK_DAMPING * (1.0f/timeDelta);

    g_PlayerPosition += g_PlayerVelocity * timeDelta;

    //Log("%f", g_PlayerPosition.x);
}

/*
void OnMouseButtonAction( int button, bool pressed )
{
    //Log("mouse button action");
}

void OnMouseScroll( double xoffset, double yoffset )
{
    //Log("mouse scroll");
}

void OnCursorMove( double x, double y, double xoffset, double yoffset )
{
    Log("%f %f", xoffset, yoffset);
    g_PlayerRotation.x += xoffset;
    g_PlayerRotation.y += yoffset;
}

void OnKeyAction( int key, int scancode, bool pressed )
{
    if(key == GLFW_KEY_ESCAPE && pressed)
}
*/
