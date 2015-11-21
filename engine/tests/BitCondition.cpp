#include <engine/BitCondition.h>
#include "TestTools.h"

static const BitCondition conditionsA[] =
{
    {0,0,111},
    {0,0,222},
    {0,0,333}
};

static const BitCondition conditionsB[] =
{
    {0,0,111},
    {0,0,444},
    {0,0,555}
};

static const BitCondition conditionsC[] =
{
    {0,0,111},
    {0,0,444}
};

static char payloadA[] = "A";
static char payloadB[] = "B";
static char payloadC[] = "C";
static char payloadD[] = "D";

static bool HasPayload( void** results, int resultCount, void* payload )
{
    for(int i = 0; i < resultCount; i++)
        if(results[i] == payload)
            return true;
    return false;
}

InlineTest("BitConditionSolver can gather matching payloads.", dummySignalSandbox)
{
    BitConditionSolver* solver = CreateBitConditionSolver();
    AddBitConditions(solver, conditionsA, 3, (void*)payloadA);
    AddBitConditions(solver, conditionsB, 3, (void*)payloadB);
    AddBitConditions(solver, conditionsC, 2, (void*)payloadC);
    AddBitConditions(solver, conditionsC, 2, (void*)payloadD);

    void** results = NULL;
    int resultCount = GatherPayloadFromBitField(solver, NULL, 0, &results);

    Require(resultCount == 4);
    Require(results != NULL);
    Require(HasPayload(results, resultCount, payloadA));
    Require(HasPayload(results, resultCount, payloadB));
    Require(HasPayload(results, resultCount, payloadC));
    Require(HasPayload(results, resultCount, payloadD));

    FreeBitConditionSolver(solver);
}

int main( int argc, char** argv )
{
    InitTests(argc, argv);
    return RunTests();
}
