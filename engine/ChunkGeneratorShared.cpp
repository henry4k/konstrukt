#include <stdlib.h> // malloc, free, realloc
#include <string.h> // memset, memcmp, memcpy

#include "ChunkGeneratorShared.h"


struct BitConditionNode
{
    BitCondition condition;

    BitConditionNode* children;
    int childCount;

    void** payloadList;
    int payloadCount;
};

struct BitConditionSolver
{
    BitConditionNode root;
};


BitConditionSolver* CreateBitConditionSolver()
{
    BitConditionSolver* solver = new BitConditionSolver;
    memset(solver, 0, sizeof(BitConditionSolver));
    return solver;
}

void FreeBitConditionSolver( BitConditionSolver* solver )
{
    delete solver;
}

static bool AreBitConditionsEqual( const BitCondition* a,
                                   const BitCondition* b )
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
        BitConditionNode* childNode = NULL;
        for(int i = 0; i < currentNode->childCount; i++)
        {
            if(AreBitConditionsEqual(&currentNode->children[i].condition,
                                     &conditions[0]))
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
            childNode->condition = conditions[0];
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

static bool IsConditionMatchingBitField( const BitCondition* condition,
                                         const void* bitField,
                                         int bitFieldSize )
{
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
