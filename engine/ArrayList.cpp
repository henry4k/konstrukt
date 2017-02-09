#include <assert.h>
#include <stdlib.h> // realloc
#include <string.h> // memmove

#include "ArrayList.h"


void _ReserveArrayListCapacity( CharArrayList* list,
                                int elementSize,
                                int newCapacity )
{
    assert(elementSize > 0);
    assert(newCapacity >= 0);
    if(newCapacity > list->capacity)
    {
        list->data = (char*)realloc((void*)list->data, elementSize*newCapacity);
        list->capacity = newCapacity;
    }
}

int _AllocateInArrayList( CharArrayList* list,
                          int elementSize,
                          int pos,
                          int amount )
{
    assert(elementSize > 0);
    assert(pos >= 0);
    assert(pos <= list->length);
    assert(amount > 0);

    _ReserveArrayListCapacity(list, elementSize, list->length+amount);

    char* dst = &list->data[(pos + amount)*elementSize];
    char* src = &list->data[pos*elementSize];
    const size_t count = (list->length - pos)*elementSize;
    memmove(dst, src, count);
    list->length += amount;

    return pos;
}

int _InsertInArrayList( CharArrayList* list,
                        int elementSize,
                        int pos,
                        int amount,
                        const char* elements )
{
    _AllocateInArrayList(list, elementSize, pos, amount);
    char* dst = &list->data[pos*elementSize];
    memcpy(dst, elements, elementSize*amount);
    return pos;
}

void _RemoveFromArrayList( CharArrayList* list,
                           int elementSize,
                           int pos,
                           int amount )
{
    assert(elementSize > 0);
    assert(pos >= 0);
    assert(pos < list->length);
    assert(amount > 0);
    assert(amount < (list->length-pos));

    char* dst = &list->data[pos*elementSize];
    char* src = &list->data[(pos + amount)*elementSize];
    const size_t count = (list->length - pos)*elementSize;
    memmove(dst, src, count);
    list->length -= amount;
}
