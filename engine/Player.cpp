#include "Common.h"
#include "OpenGL.h"
#include "Controls.h"
#include "Audio.h"
#include "Player.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

static const float MAX_MOVEMENT_SPEED = 100;
static const float MOVEMENT_ACCELERATION = 30;
//static const float MOVEMENT_ACCELERATION = 7;
static const float MOVEMENT_FRICTION = 6;

static bool g_ForwardKey;
static bool g_BackwardKey;
static bool g_RightKey;
static bool g_LeftKey;
static float g_LookX;
static float g_LookY;

static glm::vec3 g_PlayerPosition;
static glm::vec3 g_PlayerVelocity;
static glm::quat g_PlayerOrientation;
static glm::mat4 g_PlayerViewMatrix;
static glm::mat4 g_PlayerProjectionMatrix;

bool InitPlayer()
{
    g_PlayerPosition = glm::vec3(0,0,0);
    g_PlayerVelocity = glm::vec3(0,0,0);

    RegisterKeyControl("forward",  NULL, NULL, &g_ForwardKey) &&
    RegisterKeyControl("backward", NULL, NULL, &g_BackwardKey) &&
    RegisterKeyControl("right",    NULL, NULL, &g_RightKey) &&
    RegisterKeyControl("left",     NULL, NULL, &g_LeftKey) &&
    RegisterAxisControl("look-x",  NULL, NULL, &g_LookX) &&
    RegisterAxisControl("look-y",  NULL, NULL, &g_LookY);

    return true;
}

void DestroyPlayer()
{
}

void UpdatePlayerViewMatrix()
{
    using namespace glm;

    g_PlayerViewMatrix = lookAt(
        vec3(0,0,0),
        g_PlayerOrientation*vec3(0,0,1),
        vec3(0,1,0)
    );

    g_PlayerViewMatrix = translate(
        g_PlayerViewMatrix,
        vec3(
            -g_PlayerPosition.x,
            -g_PlayerPosition.y,
            -g_PlayerPosition.z
        )
     );
}

glm::mat4 GetPlayerViewMatrix()
{
    return g_PlayerViewMatrix;
}

void UpdateProjectionMatrix( int width, int height )
{
    using namespace glm;

    const float aspect = float(width) / float(height);
    g_PlayerProjectionMatrix = perspective(glm::radians(90.0f), aspect, 0.1f, 100.0f);
}

glm::mat4 GetPlayerProjectionMatrix()
{
    return g_PlayerProjectionMatrix;
}

void DrawPlayer()
{
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


    // --- Final steps ---
    g_PlayerPosition += g_PlayerVelocity * timeFrame;
    g_PlayerVelocity *= 1.0f - timeFrame * MOVEMENT_FRICTION;


    // --- Update view matrix ---
    UpdatePlayerViewMatrix();

    // --- Update listener ---
    //UpdateAudioListener(g_PlayerPosition, g_PlayerVelocity, g_PlayerOrientation*glm::vec3(0,0,1), glm::vec3(0,1,0));
}

glm::vec3 GetPlayerPosition()
{
    return g_PlayerPosition;
}

glm::quat GetPlayerOrientation()
{
    return g_PlayerOrientation;
}
