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
static const glm::vec3 PLAYER_HALF_WIDTH(0.25, 0.7, 0.25);

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
        RegisterKeyControl("forward",  NULL, NULL, &g_ForwardKey) &&
        RegisterKeyControl("backward", NULL, NULL, &g_BackwardKey) &&
        RegisterKeyControl("right",    NULL, NULL, &g_RightKey) &&
        RegisterKeyControl("left",     NULL, NULL, &g_LeftKey) &&
        RegisterAxisControl("look-x",  NULL, NULL, &g_LookX) &&
        RegisterAxisControl("look-y",  NULL, NULL, &g_LookY);
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
        -g_PlayerPosition.y,
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

void UpdatePlayer( float timeDelta )
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

    // --- Update velocity and position
    g_PlayerVelocity += direction * (MOVEMENT_ACCELERATION * timeDelta);

    if(g_PlayerVelocity.length() > MAX_MOVEMENT_SPEED)
        g_PlayerVelocity *= MAX_MOVEMENT_SPEED / g_PlayerVelocity.length();

    g_PlayerPosition += g_PlayerVelocity * timeDelta;

    g_PlayerVelocity *= 1.0f - timeDelta * MOVEMENT_FRICTION;

    // --- Resolve collisions ---
    Box playerBox;
    playerBox.position  = g_PlayerPosition;
    playerBox.halfWidth = PLAYER_HALF_WIDTH;
    playerBox.velocity  = g_PlayerVelocity;

    SimulateBoxInMap(&playerBox, timeDelta);
    g_PlayerPosition = playerBox.position;
    g_PlayerVelocity = playerBox.velocity;

    // --- Final steps ---
    g_PlayerVelocity.y = 0;
    g_PlayerPosition.y = 1.4;

    // --- Update listener ---
    UpdateAudioListener(g_PlayerPosition, g_PlayerVelocity, g_PlayerOrientation*glm::vec3(0,0,1), glm::vec3(0,1,0));
}
