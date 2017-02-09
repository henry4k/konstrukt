#ifndef __KONSTRUKT_ARRAY_LIST__
#define __KONSTRUKT_ARRAY_LIST__

#include <stdlib.h> // free
#include <string.h> // memset


#define ArrayList( elementType ) \
    struct \
    { \
        int length; \
        int capacity; \
        elementType* data; \
    }


#define InitArrayList( list ) \
    memset((list), 0, sizeof(*(list)))

#define DestroyArrayList( list ) \
    (free((list)->data), \
     InitArrayList(list))

#define AllocateInArrayList( list, pos, amount ) \
    &(list)->data[_AllocateInArrayList(_UnpackArrayList(list), (pos), (amount))]

#define AllocateAtEndOfArrayList( list, amount ) \
    &(list)->data[_AllocateInArrayList(_UnpackArrayList(list), (list)->length, (amount))]

#define InsertInArrayList( list, pos, amount, elements ) \
    &(list)->data[_InsertInArrayList(_UnpackArrayList(list), (pos), (amount), (const char*)(elements))]

#define AppendToArrayList( list, amount, elements ) \
    &(list)->data[_InsertInArrayList(_UnpackArrayList(list), (list)->length, (amount), (const char*)(elements))]

#define RemoveFromArrayList( list, pos, amount ) \
    _RemoveFromArrayList(_UnpackArrayList(list), (pos), (amount))

#define ReserveArrayListCapacity( list, newCapacity ) \
    _ReserveArrayListCapacity(_UnpackArrayList(list), (newCapacity))

#define ClearArrayList( list ) \
    ((list)->length = 0)


// --- Implementation details ---

typedef ArrayList(char) CharArrayList;

#define _UnpackArrayList( list ) \
    (CharArrayList*)(list), sizeof(*(list)->data)

void _ReserveArrayListCapacity( CharArrayList* list,
                                int elementSize,
                                int newCapacity );

int _AllocateInArrayList( CharArrayList* list,
                          int elementSize,
                          int pos,
                          int amount );

int _InsertInArrayList( CharArrayList* list,
                        int elementSize,
                        int pos,
                        int amount,
                        const char* elements );

void _RemoveFromArrayList( CharArrayList* list,
                           int elementSize,
                           int pos,
                           int amount );

#endif
