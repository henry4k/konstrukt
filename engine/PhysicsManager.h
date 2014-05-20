#ifndef __APOAPSIS_PHYSICS_MANAGER__
#define __APOAPSIS_PHYSICS_MANAGER__

#include "Math.h"


enum PhysicsFlags
{
    // nothing here yet
};

struct PhysicsObject
{
    bool active;
    glm::vec3 position;
    int physicsFlags;
    Aabb* collisionShapes;
    int collisionShapeCount;
};

bool InitPhysicsManager();
void DestroyPhysicsManager();
void UpdatePhysicsManager( double timeDelta );

PhysicsObject* CreatePhysicsObject();
void FreePhysicsObject( PhysicsObject* object );

#endif
