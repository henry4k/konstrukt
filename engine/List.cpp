#include <assert.h>
#include <stdlib.h> // malloc, realloc, free
#include <string.h> // memset, memcpy, memmove

#include "List.h"


struct List
{
    int elementSize;
    int count;
    char* elements;
};


List* CreateList( int elementSize )
{
    List* list = (List*)malloc(sizeof(List));
    memset(list, 0, sizeof(List));
    list->elementSize = elementSize;
    return list;
}

void FreeList( List* list )
{
    if(list->elements)
        free(list->elements);
    memset(list, 0, sizeof(List));
    free(list);
}

int GetListLength( const List* list )
{
    return list->count;
}

void* GetListEntry( List* list, int position )
{
    assert(position >= 0);
    assert(position < list->count);
    return &list->elements[list->elementSize * position];
}

const void* GetConstListEntry( const List* list, int position )
{
    assert(position >= 0);
    assert(position < list->count);
    return &list->elements[list->elementSize * position];
}

void* InsertListEntries( List* list, int position, int count, const void* elements )
{
    assert(position >= 0);
    assert(position <= list->count); // (position == list->count) appends elements
    assert(count > 0);
    const int elementSize = list->elementSize;
    const int oldElementCount = list->count;
    list->count += count;
    list->elements = (char*)realloc(list->elements, elementSize * list->count);
    char* insertionStart = &list->elements[elementSize*(position)];
    char* insertionEnd   = &list->elements[elementSize*(position+count)];
    if(position != oldElementCount)
    {
        // Make a hole for the new elements:
        const int elementsToMove = oldElementCount - position;
        memmove(insertionEnd, insertionStart, elementSize*elementsToMove);
    }
    if(elements != NULL)
        memcpy(insertionStart, elements, elementSize*count);
    return insertionStart;
}

void RemoveListEntries( List* list, int position, int count )
{
    assert(position >= 0);
    assert(position < list->count);
    assert(count > 0);
    assert(position+count <= list->count);
    const int elementSize = list->elementSize;
    const int oldElementCount = list->count;
    char* removalStart = &list->elements[elementSize*(position)];
    char* removalEnd   = &list->elements[elementSize*(position+count)];
    if(position != oldElementCount)
    {
        // Close the hole, created by the removed elements:
        const int elementsToMove = oldElementCount - position - count;
        memmove(removalStart, removalEnd, elementSize*elementsToMove);
    }
    list->count -= count;
    list->elements = (char*)realloc(list->elements, elementSize * list->count);
}


// ---- Utils ----

void* AppendListEntries( List* list, int count, const void* elements )
{
    return InsertListEntries(list, list->count, count, elements);
}

void* AppendListToList( List* destination, const List* source )
{
    assert(destination->elementSize == source->elementSize);
    const int length = GetListLength(source);
    if(length > 0)
        return AppendListEntries(destination, length, source->elements);
    else
        return NULL;
}
