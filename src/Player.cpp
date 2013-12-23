#include "Common.h"
#include "Math.h"
#include "OpenGL.h"
#include "Controls.h"
#include "Map.h"
#include "Audio.h"
#include "Debug.h"
#include "Player.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

static const float MAX_MOVEMENT_SPEED = 100;
static const float MOVEMENT_ACCELERATION = 30;
//static const float MOVEMENT_ACCELERATION = 7;
static const float MOVEMENT_FRICTION = 6;
static const glm::vec3 PLAYER_HALF_WIDTH(0.3, 0.8, 0.3);
static const float PLAYER_HEAD_OFFSET = 0.6;

bool g_ForwardKey;
bool g_BackwardKey;
bool g_RightKey;
bool g_LeftKey;
float g_LookX;
float g_LookY;

glm::vec3 g_PlayerPosition;
glm::vec3 g_PlayerVelocity;
glm::quat g_PlayerOrientation;

bool InitPlayer()
{
    g_PlayerPosition = glm::vec3(0,0,0);
    g_PlayerVelocity = glm::vec3(0,0,0);

    return
        RegisterKeyControl("forward",  NULL, &g_ForwardKey) &&
        RegisterKeyControl("backward", NULL, &g_BackwardKey) &&
        RegisterKeyControl("right",    NULL, &g_RightKey) &&
        RegisterKeyControl("left",     NULL, &g_LeftKey) &&
        RegisterAxisControl("look-x",  NULL, &g_LookX) &&
        RegisterAxisControl("look-y",  NULL, &g_LookY);
}

void DestroyPlayer()
{
}

void RotateWorld()
{
    glMultMatrixf(glm::value_ptr(glm::lookAt(
        glm::vec3(0,0,0),
        g_PlayerOrientation*glm::vec3(0,0,1),
        glm::vec3(0,1,0)
    )));
}

void TranslateWorld()
{
    glTranslatef(
        -g_PlayerPosition.x,
        -(g_PlayerPosition.y + PLAYER_HEAD_OFFSET),
        -g_PlayerPosition.z
    );
}

void DrawPlayer()
{
    if(IsDebugging(DEBUG_COLLISION))
    {
        Box playerBox;
        playerBox.position  = g_PlayerPosition;
        playerBox.halfWidth = PLAYER_HALF_WIDTH;
        playerBox.velocity  = g_PlayerVelocity;

        DrawBoxCollisionInMap(&playerBox);
    }
}

void UpdatePlayer( float timeFrame )
{
    using namespace glm;

    // --- Update orientation
    static const vec3 up(0,1,0);
    static const vec3 right(1,0,0);

    const quat xRotation = angleAxis(radians(-g_LookX), up);
    const quat yRotation = angleAxis(radians(g_LookY), right);

    g_PlayerOrientation = xRotation;
    g_PlayerOrientation = g_PlayerOrientation * yRotation;
    g_PlayerOrientation = normalize(g_PlayerOrientation);

    // --- Calculate walk direction
    vec3 direction(0,0,0);

    if(g_ForwardKey)
        direction.z += 1;
    if(g_BackwardKey)
        direction.z -= 1;
    if(g_LeftKey)
        direction.x += 1;
    if(g_RightKey)
        direction.x -= 1;

    if(length(direction) > 0.1f)
        direction = normalize(g_PlayerOrientation * normalize(direction));

    // --- Update velocity
    g_PlayerVelocity += direction * (MOVEMENT_ACCELERATION * timeFrame);

    if(g_PlayerVelocity.length() > MAX_MOVEMENT_SPEED)
        g_PlayerVelocity *= MAX_MOVEMENT_SPEED / g_PlayerVelocity.length();


    // --- Resolve collisions ---
    Box playerBox;
    playerBox.position  = g_PlayerPosition;
    playerBox.halfWidth = PLAYER_HALF_WIDTH;
    playerBox.velocity  = g_PlayerVelocity;

    SimulateBoxInMap(&playerBox, timeFrame);
    g_PlayerPosition = playerBox.position;
    g_PlayerVelocity = playerBox.velocity;


    // --- Final steps ---
    g_PlayerVelocity.y = 0;
    g_PlayerPosition.y = PLAYER_HALF_WIDTH.y;

    //g_PlayerPosition += g_PlayerVelocity * timeFrame;
    g_PlayerVelocity *= 1.0f - timeFrame * MOVEMENT_FRICTION;


    // --- Update listener ---
    UpdateAudioListener(g_PlayerPosition, g_PlayerVelocity, g_PlayerOrientation*glm::vec3(0,0,1), glm::vec3(0,1,0));
}
