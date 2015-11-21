#include <assert.h>
#include <math.h> // floor, ceil
#include <stdlib.h> // malloc, free, realloc
#include <string.h> // memset, memcmp, memcpy

#include "ChunkGeneratorShared.h"


static const int MAX_BIT_CONDITION_BYTES = 2;
static const int BITS_PER_BYTE = 8;


typedef unsigned char Byte;

struct OptimizedBitCondition
{
    Byte values[MAX_BIT_CONDITION_BYTES];
    Byte  masks[MAX_BIT_CONDITION_BYTES];
    int byteCount;
    int byteOffset;
};

struct BitConditionNode
{
    OptimizedBitCondition condition;

    BitConditionNode* children;
    int childCount;

    void** payloadList;
    int payloadCount;
};

struct BitConditionSolver
{
    BitConditionNode root;
};


static void FreeBitConditionNodeContents( BitConditionNode* node );

BitConditionSolver* CreateBitConditionSolver()
{
    BitConditionSolver* solver = new BitConditionSolver;
    memset(solver, 0, sizeof(BitConditionSolver));
    return solver;
}

void FreeBitConditionSolver( BitConditionSolver* solver )
{
    FreeBitConditionNodeContents(&solver->root);
    delete solver;
}

static int ceili( int a, int b )
{
    return ceilf((float)a / (float)b);
}

static int floori( int a, int b )
{
    return floorf((float)a / (float)b);
}

static Byte CreateMask( int start, int length )
{
    assert(length >= 0 && length <= BITS_PER_BYTE);
    assert(start + length <= BITS_PER_BYTE);
    const Byte bits = (1<<length)-1;
    return bits << start;
}

// TODO: Refactor this mess :(
static OptimizedBitCondition OptimizeBitCondition( const BitCondition in )
{
    assert(in.length <= MAX_BIT_CONDITION_BYTES*BITS_PER_BYTE);
    OptimizedBitCondition out;
    memset(&out, 0, sizeof(out));
    out.byteCount = ceili(in.offset+in.length, BITS_PER_BYTE) -
                    floori(in.offset, BITS_PER_BYTE);
    out.byteOffset = floorf((float)in.offset / (float)BITS_PER_BYTE);
    assert(out.byteCount >= 0); // == 0 could be used for funny hacks.
    assert(out.byteCount <= MAX_BIT_CONDITION_BYTES);
    for(int i = 0; i < out.byteCount; i++)
    {
        Byte value;
        Byte mask;
        if(out.byteCount == 1)
        {
            const int start = in.offset % BITS_PER_BYTE;
            const int end = (in.offset + in.length) % BITS_PER_BYTE;
            mask = CreateMask(start, BITS_PER_BYTE-start) &
                   CreateMask(0, end);
            value = in.value << start;
        }
        else if(i == 0) // first
        {
            const int start = in.offset % BITS_PER_BYTE;
            mask = CreateMask(start, BITS_PER_BYTE-start);
            value = in.value << start;
        }
        else if(i == out.byteCount-1) // last
        {
            const int end = (in.offset + in.length) % BITS_PER_BYTE;
            mask = CreateMask(0, end);
            value = in.value >> (in.length - end);
        }
        else // something in the middle
        {
            mask = CreateMask(0, BITS_PER_BYTE);
            value = in.value >> (i*BITS_PER_BYTE);
        }
        out.masks[i]  = mask;
        out.values[i] = value;
    }
    return out;
}

static bool AreBitConditionsEqual( const OptimizedBitCondition* a,
                                   const OptimizedBitCondition* b )
{
    return memcmp(a, b, sizeof(BitCondition)) == 0;
}

static void DepositPayload( BitConditionNode* currentNode,
                            const BitCondition* conditions,
                            int conditionCount,
                            void* payload )
{
    if(conditionCount == 0)
    {
        currentNode->payloadCount++;
        const int count = currentNode->payloadCount;
        currentNode->payloadList = (void**)realloc(currentNode->payloadList,
                                                   sizeof(void*)*count);
        currentNode->payloadList[count-1] = payload;
    }
    else
    {
        const OptimizedBitCondition optimizedCondition =
            OptimizeBitCondition(conditions[0]);
        BitConditionNode* childNode = NULL;
        for(int i = 0; i < currentNode->childCount; i++)
        {
            if(AreBitConditionsEqual(&currentNode->children[i].condition,
                                     &optimizedCondition))
            {
                childNode = &currentNode->children[i];
                break;
            }
        }
        if(!childNode)
        {
            currentNode->childCount++;
            const int count = currentNode->childCount;
            currentNode->children =
                (BitConditionNode*)realloc(currentNode->children,
                                           sizeof(BitConditionNode)*count);

            childNode = &currentNode->children[count-1];
            memset(childNode, 0, sizeof(BitConditionNode));
            childNode->condition = optimizedCondition;
        }

        const BitCondition* nextCondition;
        if(conditionCount-1 == 0)
            nextCondition = NULL;
        else
            nextCondition = &conditions[1];

        DepositPayload(childNode,
                       nextCondition,
                       conditionCount-1,
                       payload);
    }
}

static void FreeBitConditionNodeContents( BitConditionNode* node )
{
    for(int i = 0; i < node->childCount; i++)
        FreeBitConditionNodeContents(&node->children[i]);
    if(node->children)
        delete[] node->children;
    if(node->payloadList)
        delete[] node->payloadList;
}

void AddBitConditions( BitConditionSolver* solver,
                       const BitCondition* conditions,
                       int conditionCount,
                       void* payload )
{
    DepositPayload(&solver->root,
                   conditions,
                   conditionCount,
                   payload);
}

static bool IsConditionMatchingBitField( const OptimizedBitCondition* condition,
                                         const void* bitField,
                                         int bitFieldByteCount )
{
    if(condition->byteOffset + condition->byteCount > bitFieldByteCount)
        return false;
    for(int i = 0; i < condition->byteCount; i++)
    {
        const Byte bitFieldByte =
            ((const Byte*)bitField)[condition->byteOffset + i];
        if((bitFieldByte & condition->masks[i]) != condition->values[i])
            return false;
    }
    return true;
}

static void FindMatchingBitConditions( const BitConditionNode* node,
                                       const void* bitField,
                                       int bitFieldSize,
                                       void*** results,
                                       int* resultCount )
{
    if(IsConditionMatchingBitField(&node->condition,
                                   bitField,
                                   bitFieldSize))
    {
        // Append payloads to result list:
        const int start = *resultCount;
        *resultCount += node->payloadCount;
        *results = (void**)realloc(*results,
                                   sizeof(void*)*(*resultCount));
        for(int i = 0; i < node->payloadCount; i++)
            (*results)[start+i] = node->payloadList[i];

        // Try to match child nodes:
        for(int i = 0; i < node->childCount; i++)
            FindMatchingBitConditions(&node->children[i],
                                      bitField,
                                      bitFieldSize,
                                      results,
                                      resultCount);
    }
}

int GatherPayloadFromBitField( const BitConditionSolver* solver,
                               const void* bitField,
                               int bitFieldSize,
                               void*** payloadList )
{
    int resultCount = 0;
    FindMatchingBitConditions(&solver->root,
                              bitField,
                              bitFieldSize,
                              payloadList,
                              &resultCount);
    return resultCount;
}
