#include <stdlib.h> // atoi
#include <string.h> // strlen, strncmp

#include "../Common.h"
#include "../ObjectSystem.h"


static const int THING_VALUE_COUNT = 16;

struct Thing
{
    long values[THING_VALUE_COUNT];
};

struct Config
{
    const char* testName;
    int loopCount;
    int objectCount;
};

static void CreateModifyDestroy( const Config c )
{
    ObjectSystem<Thing> sys;
    InitObjectSystem(&sys);
    ObjectId* ids = (ObjectId*)Alloc(sizeof(ObjectId)*c.objectCount);
    REPEAT(c.loopCount,_)
    {
        // Create objects:
        REPEAT(c.objectCount, i)
        {
            const ObjectId id = AllocateObject(&sys);
            ids[i] = id;

            Thing* thing = GetObject(&sys, id);
            REPEAT(THING_VALUE_COUNT, j)
                thing->values[j] = i*100 + j;
        }

        // Modify objects:
        REPEAT(c.objectCount, i)
        {
            const ObjectId id = ids[i];
            Thing* thing = GetObject(&sys, id);
            REPEAT(THING_VALUE_COUNT, j)
                thing->values[j]++;
        }

        // Remove objects:
        REPEAT(c.objectCount, i)
        {
            const ObjectId id = ids[i];
            RemoveObject(&sys, id);
        }
    }
    Free(ids);
    DestroyObjectSystem(&sys);
}

static void Iterate( const Config c )
{
    ObjectSystem<Thing> sys;
    InitObjectSystem(&sys);

    // Create objects:
    REPEAT(c.objectCount, i)
    {
        const ObjectId id = AllocateObject(&sys);
        Thing* thing = GetObject(&sys, id);
        REPEAT(THING_VALUE_COUNT, j)
            thing->values[j] = i*100 + j;
    }

    REPEAT(c.loopCount,_)
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

static const char* MatchPrefix( const char* prefix, const char* value )
{
    const size_t prefixLength = strlen(prefix);
    if(strncmp(prefix, value, prefixLength) == 0)
        return &value[prefixLength];
    else
        return NULL;
}

int main( int argc, char** argv )
{
    Config c = {};
    c.testName = "";
    c.loopCount = 1;
    c.objectCount = MAX_OBJECTS;

    for(int i = 1; i < argc; i++)
    {
        const char* arg = argv[i];
        const char* match;

        match = MatchPrefix("--loop-count=", arg);
        if(match) { c.loopCount = atoi(match); continue; }

        match = MatchPrefix("--object-count=", arg);
        if(match) { c.objectCount = atoi(match); continue; }

        c.testName = arg;
    }

    LogNotice("test: %s", c.testName);
    LogNotice("loop-count: %d", c.loopCount);
    LogNotice("object-count: %d", c.objectCount);

    if(strcmp(c.testName, "CreateModifyDestroy") == 0)
        CreateModifyDestroy(c);
    else if(strcmp(c.testName, "Iterate") == 0)
        Iterate(c);
    else
        FatalError("No such test: %s", c.testName);

    return 0;
}
