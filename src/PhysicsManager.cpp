#include <string.h> // memset

#include "Common.h"
#include "Lua.h"
#include "PhysicsManager.h"


static const int MAX_PHYSICS_OBJECTS = 8;
static PhysicsObject PhysicsObjects[MAX_PHYSICS_OBJECTS];

bool InitPhysicsManager()
{
    memset(PhysicsObjects, 0, sizeof(PhysicsObjects));
    return true;
}

void DestroyPhysicsManager()
{
    for(int i = 0; i < MAX_PHYSICS_OBJECTS; i++)
        if(PhysicsObjects[i].active)
            Error("Physics object #%d (%p) was still active when the manager was destroyed.",
                i, &PhysicsObjects[i]);
}

static void UpdatePhysicsObject( PhysicsObject* object )
{
    if(!object->active)
        return;

    // TODO: Run simulation here!
}

void UpdatePhysicsManager( double timeDelta )
{
    for(int i = 0; i < MAX_PHYSICS_OBJECTS; i++)
        UpdatePhysicsObject(&PhysicsObjects[i]);
}

static PhysicsObject* FindInactivePhysicsObject()
{
    for(int i = 0; i < MAX_PHYSICS_OBJECTS; i++)
        if(!PhysicsObjects[i].active)
            return &PhysicsObjects[i];
    return NULL;
}

PhysicsObject* CreatePhysicsObject()
{
    PhysicsObject* object = FindInactivePhysicsObject();
    if(object)
    {
        memset(object, 0, sizeof(PhysicsObject));
        object->active = true;
        return object;
    }
    else
    {
        Error("Can't create more physics objects.");
        return NULL;
    }
}

void FreePhysicsObject( PhysicsObject* object )
{
    object->active = false;
}
