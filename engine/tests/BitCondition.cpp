#include <stdlib.h> // NULL
#include "../Common.h"
#include "../BitCondition.h"
#include "TestTools.h"
#include <dummy/inline.hpp>

#define InlineTest DUMMY_INLINE_TEST

static bool HasPayload( const BitFieldPayloadList results, void* payload )
{
    REPEAT(results.length, i)
        if(results.data[i] == payload)
            return true;
    return false;
}

InlineTest("simple single condition", dummySignalSandbox)
{
    const BitCondition condition = {0, 4, 0xF}; // first 4 bits are set
    char payload = 'A';

    BitConditionSolver* solver = CreateBitConditionSolver();
    AddBitConditions(solver, &condition, 1, &payload);

    // invalid bit field 1
    {
        const unsigned int bitField = 0x0;
        BitFieldPayloadList result =
            GatherPayloadFromBitField(solver, &bitField, sizeof(bitField));

        Require(result.length == 0);
        Require(result.data == NULL); // Not a strict requirement, but why should anything be allocated?
    }

    // invalid bit field 2
    {
        const unsigned int bitField = 0xF0;
        BitFieldPayloadList result =
            GatherPayloadFromBitField(solver, &bitField, sizeof(bitField));

        Require(result.length == 0);
        Require(result.data == NULL);
    }

    // invalid bit field 3
    {
        const unsigned int bitField = 0x1;
        BitFieldPayloadList result =
            GatherPayloadFromBitField(solver, &bitField, sizeof(bitField));

        Require(result.length == 0);
        Require(result.data == NULL);
    }

    // invalid bit field 4
    {
        const unsigned int bitField = 0xE;
        BitFieldPayloadList result =
            GatherPayloadFromBitField(solver, &bitField, sizeof(bitField));

        Require(result.length == 0);
        Require(result.data == NULL);
    }

    // valid bit field
    {
        const unsigned int bitField = 0xF;
        BitFieldPayloadList result =
            GatherPayloadFromBitField(solver, &bitField, sizeof(bitField));

        Require(result.length == 1);
        Require(result.data[0] == &payload);
    }

    FreeBitConditionSolver(solver);
}

InlineTest("complex single condition", dummySignalSandbox)
{
    const BitCondition condition = {4, 4, 0xF}; // second 4 bit group is 0xF
    char payload = 'A';

    BitConditionSolver* solver = CreateBitConditionSolver();
    AddBitConditions(solver, &condition, 1, &payload);

    // invalid bit field 1
    {
        const unsigned int bitField = 0x00;
        BitFieldPayloadList result =
            GatherPayloadFromBitField(solver, &bitField, sizeof(bitField));

        Require(result.length == 0);
        Require(result.data == NULL); // Not a strict requirement, but why should anything be allocated?
    }

    // invalid bit field 2
    {
        const unsigned int bitField = 0x0F;
        BitFieldPayloadList result =
            GatherPayloadFromBitField(solver, &bitField, sizeof(bitField));

        Require(result.length == 0);
        Require(result.data == NULL);
    }

    // invalid bit field 3
    {
        const unsigned int bitField = 0x1F;
        BitFieldPayloadList result =
            GatherPayloadFromBitField(solver, &bitField, sizeof(bitField));

        Require(result.length == 0);
        Require(result.data == NULL);
    }

    // valid bit field 1
    {
        const unsigned int bitField = 0xF0;
        BitFieldPayloadList result =
            GatherPayloadFromBitField(solver, &bitField, sizeof(bitField));

        Require(result.length == 1);
        Require(result.data[0] == &payload);

        DestroyBitFieldPayloadList(&result);
    }

    // valid bit field 2
    {
        const unsigned int bitField = 0xFF;
        BitFieldPayloadList result =
            GatherPayloadFromBitField(solver, &bitField, sizeof(bitField));

        Require(result.length == 1);
        Require(result.data[0] == &payload);

        DestroyBitFieldPayloadList(&result);
    }

    FreeBitConditionSolver(solver);
}

InlineTest("multiple single conditions", dummySignalSandbox)
{
    const BitCondition conditions[] =
    {
        {0, 4, 0xF}, // first 4 bit group is 0xF
        {4, 4, 0xF}  // second 4 bit group is 0xF
    };
    char payload = 'A';

    BitConditionSolver* solver = CreateBitConditionSolver();
    AddBitConditions(solver, conditions, 2, &payload);

    // invalid bit field 1
    {
        const unsigned int bitField = 0x000;
        BitFieldPayloadList result =
            GatherPayloadFromBitField(solver, &bitField, sizeof(bitField));

        Require(result.length == 0);
        Require(result.data == NULL);
    }

    // invalid bit field 2
    {
        const unsigned int bitField = 0x00F;
        BitFieldPayloadList result =
            GatherPayloadFromBitField(solver, &bitField, sizeof(bitField));

        Require(result.length == 0);
        Require(result.data == NULL);
    }

    // invalid bit field 3
    {
        const unsigned int bitField = 0x0F0;
        BitFieldPayloadList result =
            GatherPayloadFromBitField(solver, &bitField, sizeof(bitField));

        Require(result.length == 0);
        Require(result.data == NULL);
    }

    // invalid bit field 4
    {
        const unsigned int bitField = 0xFF0;
        BitFieldPayloadList result =
            GatherPayloadFromBitField(solver, &bitField, sizeof(bitField));

        Require(result.length == 0);
        Require(result.data == NULL);
    }

    // valid bit field 1
    {
        const unsigned int bitField = 0x0FF;
        BitFieldPayloadList result =
            GatherPayloadFromBitField(solver, &bitField, sizeof(bitField));

        Require(result.length == 1);
        Require(result.data[0] == &payload);
    }

    // valid bit field 2
    {
        const unsigned int bitField = 0xFFF;
        BitFieldPayloadList result =
            GatherPayloadFromBitField(solver, &bitField, sizeof(bitField));

        Require(result.length == 1);
        Require(result.data[0] == &payload);
    }

    FreeBitConditionSolver(solver);
}

InlineTest("multiple payloads", dummySignalSandbox)
{
    const BitCondition conditionsA[] =
    {
        {0, 4, 0xF}, // first 4 bit group is 0xF
        {4, 4, 0xF}  // second 4 bit group is 0xF
    };
    char payloadA = 'A';

    const BitCondition conditionsB[] =
    {
        {0, 4, 0xF}, // first 4 bit group is 0xF
        {8, 4, 0xF}  // third 4 bit group is 0xF
    };
    char payloadB = 'B';

    BitConditionSolver* solver = CreateBitConditionSolver();
    AddBitConditions(solver, conditionsA, 2, &payloadA);
    AddBitConditions(solver, conditionsB, 2, &payloadB);

    // invalid bit field 1
    {
        const unsigned int bitField = 0x00F;
        BitFieldPayloadList result =
            GatherPayloadFromBitField(solver, &bitField, sizeof(bitField));

        Require(result.length == 0);
        Require(result.data == NULL);
    }

    // invalid bit field 2
    {
        const unsigned int bitField = 0x0F0;
        BitFieldPayloadList result =
            GatherPayloadFromBitField(solver, &bitField, sizeof(bitField));

        Require(result.length == 0);
        Require(result.data == NULL);
    }

    // invalid bit field 3
    {
        const unsigned int bitField = 0xF00;
        BitFieldPayloadList result =
            GatherPayloadFromBitField(solver, &bitField, sizeof(bitField));

        Require(result.length == 0);
        Require(result.data == NULL);
    }

    // valid bit field 1
    {
        const unsigned int bitField = 0x0FF;
        BitFieldPayloadList result =
            GatherPayloadFromBitField(solver, &bitField, sizeof(bitField));

        Require(result.length == 1);
        Require(result.data[0] == &payloadA);
    }

    // valid bit field 2
    {
        const unsigned int bitField = 0xF0F;
        BitFieldPayloadList result =
            GatherPayloadFromBitField(solver, &bitField, sizeof(bitField));

        Require(result.length == 1);
        Require(result.data[0] == &payloadB);
    }

    // valid bit field 3
    {
        const unsigned int bitField = 0xFFF;
        BitFieldPayloadList result =
            GatherPayloadFromBitField(solver, &bitField, sizeof(bitField));

        Require(result.length == 2);
        Require(HasPayload(result, &payloadA));
        Require(HasPayload(result, &payloadB));
    }

    FreeBitConditionSolver(solver);
}

int main( int argc, char** argv )
{
    InitTests(argc, argv);
    dummyAddInlineTests();
    return RunTests();
}
