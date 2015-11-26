#include <assert.h>
#include <math.h> // floor, ceil
#include <stdlib.h> // malloc, free, realloc
#include <string.h> // memset, memcmp, memcpy

#include "List.h"
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
    List* childList;
    List* payloadList;
};

struct BitConditionSolver
{
    BitConditionNode root;
};


static void InitializeBitConditionNode( BitConditionNode* node );
static void FreeBitConditionNodeContents( BitConditionNode* node );

BitConditionSolver* CreateBitConditionSolver()
{
    BitConditionSolver* solver = new BitConditionSolver;
    memset(solver, 0, sizeof(BitConditionSolver));
    InitializeBitConditionNode(&solver->root);
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

static void InitializeBitConditionNode( BitConditionNode* node )
{
    memset(node, 0, sizeof(BitConditionNode));
    node->childList   = CreateList(sizeof(BitConditionNode));
    node->payloadList = CreateList(sizeof(void*));
}

static void DepositPayload( BitConditionNode* currentNode,
                            const BitCondition* conditions,
                            int conditionCount,
                            void* payload )
{
    if(conditionCount == 0)
    {
        AppendListEntries(currentNode->payloadList, 1, &payload);
    }
    else
    {
        const OptimizedBitCondition optimizedCondition =
            OptimizeBitCondition(conditions[0]);
        BitConditionNode* nextChild = NULL;
        const int childCount = GetListLength(currentNode->childList);
        for(int i = 0; i < childCount; i++)
        {
            BitConditionNode* child =
                (BitConditionNode*)GetListEntry(currentNode->childList, i);
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
                (BitConditionNode*)AppendListEntries(currentNode->childList, 1, NULL);
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
    const int childCount = GetListLength(node->childList);
    for(int i = 0; i < childCount; i++)
    {
        BitConditionNode* child =
            (BitConditionNode*)GetListEntry(node->childList, i);
        FreeBitConditionNodeContents(child);
    }
    FreeList(node->childList);
    FreeList(node->payloadList);
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
                                       List* resultList );

static void FindMatchingBitConditionsInChilds( const List* childList,
                                               const void* bitField,
                                               int bitFieldSize,
                                               List* resultList )
{
    const int childCount = GetListLength(childList);
    for(int i = 0; i < childCount; i++)
    {
        const BitConditionNode* child =
            (const BitConditionNode*)GetConstListEntry(childList, i);
        FindMatchingBitConditions(child,
                                  bitField,
                                  bitFieldSize,
                                  resultList);
    }
}

static void FindMatchingBitConditions( const BitConditionNode* node,
                                       const void* bitField,
                                       int bitFieldSize,
                                       List* resultList )
{
    if(IsConditionMatchingBitField(&node->condition,
                                   bitField,
                                   bitFieldSize))
    {
        // Append payloads to result list:
        AppendListToList(resultList, node->payloadList);

        // Try to match child nodes:
        FindMatchingBitConditionsInChilds(node->childList,
                                          bitField,
                                          bitFieldSize,
                                          resultList);
    }
}

List* GatherPayloadFromBitField( const BitConditionSolver* solver,
                                 const void* bitField,
                                 int bitFieldSize )
{
    List* resultList = CreateList(sizeof(void*));
    FindMatchingBitConditionsInChilds(solver->root.childList,
                                      bitField,
                                      bitFieldSize,
                                      resultList);
    return resultList;
}
