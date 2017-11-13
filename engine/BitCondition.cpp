#include <assert.h>
#include <math.h> // floor, ceil
#include <string.h> // memset, memcmp, memcpy

#include "Common.h"
#include "Array.h"
#include "BitCondition.h"


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
    Array<BitConditionNode> childList;
    Array<void*> payloadList;
};

struct BitConditionSolver
{
    BitConditionNode root;
};


static void InitializeBitConditionNode( BitConditionNode* node );
static void FreeBitConditionNodeContents( BitConditionNode* node );

BitConditionSolver* CreateBitConditionSolver()
{
    BitConditionSolver* solver = NEW(BitConditionSolver);
    InitializeBitConditionNode(&solver->root);
    return solver;
}

void FreeBitConditionSolver( BitConditionSolver* solver )
{
    FreeBitConditionNodeContents(&solver->root);
    DELETE(solver);
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
    out.byteOffset = floori(in.offset, BITS_PER_BYTE);
    out.byteCount = ceili(in.offset+in.length, BITS_PER_BYTE) - out.byteOffset;
    assert(out.byteCount >= 0); // == 0 could be used for funny hacks.
    assert(out.byteCount <= MAX_BIT_CONDITION_BYTES);
    REPEAT(out.byteCount, i)
    {
        Byte value;
        Byte mask;
        if(out.byteCount == 1)
        {
            const int start = in.offset % BITS_PER_BYTE;
            const int end = (in.offset + in.length - 1) % BITS_PER_BYTE + 1; // -1 and +1 to avoid a zero
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
            const int end = (in.offset + in.length - 1) % BITS_PER_BYTE + 1; // -1 and +1 to avoid a zero
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

static void InitializeBitConditionNode( BitConditionNode* node )
{
    memset(node, 0, sizeof(BitConditionNode));
    InitArray(&node->childList);
    InitArray(&node->payloadList);
}

static void DepositPayload( BitConditionNode* currentNode,
                            const BitCondition* conditions,
                            int conditionCount,
                            void* payload )
{
    if(conditionCount == 0)
    {
        AppendToArray(&currentNode->payloadList, 1, &payload);
    }
    else
    {
        const OptimizedBitCondition optimizedCondition =
            OptimizeBitCondition(conditions[0]);
        BitConditionNode* nextChild = NULL;
        REPEAT(currentNode->childList.length, i)
        {
            BitConditionNode* child = currentNode->childList.data + i;
            if(AreBitConditionsEqual(&child->condition,
                                     &optimizedCondition))
            {
                nextChild = child;
                break;
            }
        }
        if(!nextChild)
        {
            BitConditionNode* child =
                AllocateAtEndOfArray(&currentNode->childList, 1);
            InitializeBitConditionNode(child);
            child->condition = optimizedCondition;
            nextChild = child;
        }

        const BitCondition* nextCondition;
        if(conditionCount-1 == 0)
            nextCondition = NULL;
        else
            nextCondition = &conditions[1];

        DepositPayload(nextChild,
                       nextCondition,
                       conditionCount-1,
                       payload);
    }
}

static void FreeBitConditionNodeContents( BitConditionNode* node )
{
    REPEAT(node->childList.length, i)
    {
        BitConditionNode* child = node->childList.data + i;
        FreeBitConditionNodeContents(child);
    }
    DestroyArray(&node->childList);
    DestroyArray(&node->payloadList);
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
    REPEAT(condition->byteCount, i)
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
                                       Array<void*>* resultList );

static void FindMatchingBitConditionsInChilds( const Array<BitConditionNode>* childList,
                                               const void* bitField,
                                               int bitFieldSize,
                                               Array<void*>* resultList )
{
    REPEAT(childList->length, i)
    {
        const BitConditionNode* child = childList->data + i;
        FindMatchingBitConditions(child,
                                  bitField,
                                  bitFieldSize,
                                  resultList);
    }
}

static void FindMatchingBitConditions( const BitConditionNode* node,
                                       const void* bitField,
                                       int bitFieldSize,
                                       Array<void*>* resultList )
{
    if(IsConditionMatchingBitField(&node->condition,
                                   bitField,
                                   bitFieldSize))
    {
        // Append payloads to result list:
        const Array<void*>* payloadList = &node->payloadList;
        AppendToArray(resultList, payloadList->length, payloadList->data);

        // Try to match child nodes:
        FindMatchingBitConditionsInChilds(&node->childList,
                                          bitField,
                                          bitFieldSize,
                                          resultList);
    }
}

BitFieldPayloadList GatherPayloadFromBitField( const BitConditionSolver* solver,
                                               const void* bitField,
                                               int bitFieldSize )
{
    Array<void*> resultList;
    InitArray(&resultList);
    FindMatchingBitConditionsInChilds(&solver->root.childList,
                                      bitField,
                                      bitFieldSize,
                                      &resultList);
    BitFieldPayloadList r = {resultList.length, resultList.data};
    return r;
}

void DestroyBitFieldPayloadList( BitFieldPayloadList* list )
{
    Free(list->data);
    memset(list, 0, sizeof(BitFieldPayloadList));
}
