#include "../Common.h"
#include "../Config.h"
#include "../ObjectSystem.h"
#include "TestTools.h"


static const int THING_VALUE_COUNT = 16;

struct Thing
{
    long values[THING_VALUE_COUNT];
};

int LoopCount;
int ObjectCount;

InlineTest("CreateModifyDestroy")
{
    ObjectSystem<Thing> sys;
    InitObjectSystem(&sys);
    ObjectId* ids = (ObjectId*)Alloc(sizeof(ObjectId)*ObjectCount);

    REPEAT(LoopCount,_)
    {
        // Create objects:
        REPEAT(ObjectCount, i)
        {
            const ObjectId id = AllocateObject(&sys);
            ids[i] = id;

            Thing* thing = GetObject(&sys, id);
            REPEAT(THING_VALUE_COUNT, j)
                thing->values[j] = i*100 + j;
        }

        // Modify objects:
        REPEAT(ObjectCount, i)
        {
            const ObjectId id = ids[i];
            Thing* thing = GetObject(&sys, id);
            REPEAT(THING_VALUE_COUNT, j)
                thing->values[j]++;
        }

        // Remove objects:
        REPEAT(ObjectCount, i)
        {
            const ObjectId id = ids[i];
            RemoveObject(&sys, id);
        }
    }
    Free(ids);
    DestroyObjectSystem(&sys);
}

InlineTest("Iterate")
{
    ObjectSystem<Thing> sys;
    InitObjectSystem(&sys);

    // Create objects:
    REPEAT(ObjectCount, i)
    {
        const ObjectId id = AllocateObject(&sys);
        Thing* thing = GetObject(&sys, id);
        REPEAT(THING_VALUE_COUNT, j)
            thing->values[j] = i*100 + j;
    }

    REPEAT(LoopCount,_)
    {
        // Modify objects:
        REPEAT(GetObjectCount(&sys),i)
        {
            Thing* thing = GetObjectByIndex(&sys, i);
            REPEAT(THING_VALUE_COUNT, j)
                thing->values[j]++;
        }
    }

    // Remove objects:
    REPEAT(GetObjectCount(&sys),i)
        RemoveObject(&sys, GetObjectIdByIndex(&sys, 0));

    DestroyObjectSystem(&sys);
}

int main( int argc, char** argv )
{
    InitTests(argc, argv);
    LoopCount = GetConfigInt("test.loop-count", 1);
    ObjectCount = GetConfigInt("test.object-count", MAX_OBJECTS);
    return RunTests();
}
