#include <string.h> // memset

#include "Common.h"
#include "PhysicsManager.h"


struct Solid
{
    bool active;
    glm::vec3 position;
    Aabb* collisionShapes;
    int collisionShapeCount;
};


static const int MAX_SOLIDS = 8;
static Solid Solids[MAX_SOLIDS];

bool InitPhysicsManager()
{
    memset(Solids, 0, sizeof(Solids));
    return true;
}

void DestroyPhysicsManager()
{
    for(int i = 0; i < MAX_SOLIDS; i++)
        if(Solids[i].active)
            Error("Solid #%d (%p) was still active when the manager was destroyed.",
                i, &Solids[i]);
}

static void UpdateSolid( Solid* solid )
{
    if(!solid->active)
        return;

    // TODO: Run simulation here!
}

void UpdatePhysicsManager( double timeDelta )
{
    for(int i = 0; i < MAX_SOLIDS; i++)
        UpdateSolid(&Solids[i]);
}

static Solid* FindInactiveSolid()
{
    for(int i = 0; i < MAX_SOLIDS; i++)
        if(!Solids[i].active)
            return &Solids[i];
    return NULL;
}

Solid* CreateSolid()
{
    Solid* solid = FindInactiveSolid();
    if(solid)
    {
        memset(solid, 0, sizeof(Solid));
        solid->active = true;
        return solid;
    }
    else
    {
        Error("Can't create more solids.");
        return NULL;
    }
}

void FreeSolid( Solid* solid )
{
    solid->active = false;
}
