#ifndef __KONSTRUKT_ARRAY_LIST__
#define __KONSTRUKT_ARRAY_LIST__

#include "Common.h" // Free
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
    (Free((list)->data), \
     InitArrayList(list))

#define AllocateInArrayList( list, pos, amount ) \
    (_AllocateInArrayList(_UnpackArrayList(list), (pos), (amount)), \
     (list)->data + (pos))

#define AllocateAtEndOfArrayList( list, amount ) \
    (_AllocateInArrayList(_UnpackArrayList(list), (list)->length, (amount)), \
     (list)->data + (list)->length - (amount))

#define InsertInArrayList( list, pos, amount, elements ) \
    (_InsertInArrayList(_UnpackArrayList(list), (pos), (amount), (const char*)(elements)), \
     (list)->data + (pos))

#define AppendToArrayList( list, amount, elements ) \
    (_InsertInArrayList(_UnpackArrayList(list), (list)->length, (amount), (const char*)(elements)), \
     (list)->data + (list)->length - (amount))

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

void _AllocateInArrayList( CharArrayList* list,
                           int elementSize,
                           int pos,
                           int amount );

void _InsertInArrayList( CharArrayList* list,
                         int elementSize,
                         int pos,
                         int amount,
                         const char* elements );

void _RemoveFromArrayList( CharArrayList* list,
                           int elementSize,
                           int pos,
                           int amount );

#endif
