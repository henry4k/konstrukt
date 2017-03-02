#include "../Common.h"
#include "../ObjectSystem.h"
#include "TestTools.h"
#include <dummy/inline.hpp>

#define InlineTest DUMMY_INLINE_TEST


static const int THING_VALUE_COUNT = 16;

struct Thing
{
    long values[THING_VALUE_COUNT];
};

InlineTest("Can create and remove objects.", dummySignalSandbox)
{
    ObjectSystem<Thing> sys;
    InitObjectSystem(&sys);

    // Can create new objects:
    const ObjectId id = AllocateObject(&sys);
    Require(HasObject(&sys, id));
    Thing* thing = GetObject(&sys, id);
    Require(thing != NULL);

    // Can remove objects:
    RemoveObject(&sys, id);
    Require(!HasObject(&sys, id));

    DestroyObjectSystem(&sys);
}

InlineTest("IDs are not reused.", dummySignalSandbox)
{
    ObjectSystem<Thing> sys;
    InitObjectSystem(&sys);

    const ObjectId aId = AllocateObject(&sys);
    RemoveObject(&sys, aId);
    const ObjectId bId = AllocateObject(&sys);
    Require(bId != aId);
    RemoveObject(&sys, bId);

    DestroyObjectSystem(&sys);
}

InlineTest("Can remove single objects.", dummySignalSandbox)
{
    ObjectSystem<Thing> sys;
    InitObjectSystem(&sys);

    ObjectId ids[5];

    // Create objects:
    REPEAT(5, i)
    {
        const ObjectId id = AllocateObject(&sys);
        ids[i] = id;
        Thing* thing = GetObject(&sys, id);
        REPEAT(THING_VALUE_COUNT, j)
            thing->values[j] = i*100 + j;
    }

    // Remove the object in the middle:
    RemoveObject(&sys, ids[2]);

    // Verify remaining objects:
    REPEAT(5, i)
    {
        if(i == 2)
            continue;
        const ObjectId id = ids[i];
        Require(HasObject(&sys, id));
        const Thing* thing = GetObject(&sys, id);
        REPEAT(THING_VALUE_COUNT, j)
            Require(thing->values[j] == i*100 + j);
    }

    // Remove remaining objects:
    REPEAT(5, i)
    {
        if(i == 2)
            continue;
        const ObjectId id = ids[i];
        RemoveObject(&sys, id);
    }

    DestroyObjectSystem(&sys);
}

InlineTest("Can create MAX_OBJECTS.", dummySignalSandbox)
{
    ObjectSystem<Thing> sys;
    InitObjectSystem(&sys);

    ObjectId* ids = (ObjectId*)Alloc(sizeof(ObjectId)*MAX_OBJECTS);

    // Create objects:
    REPEAT(MAX_OBJECTS, i)
    {
        const ObjectId id = AllocateObject(&sys);
        ids[i] = id;

        Require(HasObject(&sys, id));

        Thing* thing = GetObject(&sys, id);
        REPEAT(THING_VALUE_COUNT, j)
            thing->values[j] = i*100 + j;
    }

    // Verify objects:
    REPEAT(MAX_OBJECTS, i)
    {
        const ObjectId id = ids[i];
        Require(HasObject(&sys, id));
        const Thing* thing = GetObject(&sys, id);
        REPEAT(THING_VALUE_COUNT, j)
            Require(thing->values[j] == i*100 + j);
    }

    // Remove objects:
    REPEAT(MAX_OBJECTS, i)
    {
        const ObjectId id = ids[i];
        RemoveObject(&sys, id);
        Require(!HasObject(&sys, id));
    }

    Free(ids);
    DestroyObjectSystem(&sys);
}

int main( int argc, char** argv )
{
    InitTests(argc, argv);
    dummyAddInlineTests();
    return RunTests();
}
