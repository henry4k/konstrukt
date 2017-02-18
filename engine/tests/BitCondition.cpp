#include <stdlib.h> // NULL
#include "../Common.h"
#include "../BitCondition.h"
#include "TestTools.h"
#include <dummy/inline.hpp>

#define InlineTest DUMMY_INLINE_TEST

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

static bool HasPayload( const BitFieldPayloadList results, void* payload )
{
    REPEAT(results.length, i)
        if(results.data[i] == payload)
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

    const BitFieldPayloadList results =
        GatherPayloadFromBitField(solver, NULL, 0);

    Require(results.length == 4);
    Require(HasPayload(results, payloadA));
    Require(HasPayload(results, payloadB));
    Require(HasPayload(results, payloadC));
    Require(HasPayload(results, payloadD));

    FreeBitConditionSolver(solver);
}

int main( int argc, char** argv )
{
    InitTests(argc, argv);
    dummyAddInlineTests();
    return RunTests();
}
