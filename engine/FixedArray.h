#ifndef __KONSTRUKT_FIXED_ARRAY__
#define __KONSTRUKT_FIXED_ARRAY__

#include "Common.h"
#include "Array.h"


template<typename T>
struct FixedArraySlot
{
    bool inUse;
    T element;
};

template<typename T>
struct FixedArray
{
    Array<FixedArraySlot<T>> _;
};

template<typename T>
struct FixedArrayAllocation
{
    int pos;
    T* element;
};


template<typename T>
void InitFixedArray( FixedArray<T>* array ) { InitArray(&array->_); }

template<typename T>
void DestroyFixedArray( FixedArray<T>* array ) { DestroyArray(&array->_); }

template<typename T>
void ClearFixedArray( FixedArray<T>* array ) { ClearArray(&array->_); }

template<typename T>
T* GetFixedArrayElement( FixedArray<T>* array, int pos )
{
    Ensure(pos >= 0 && pos < array->_.length);
    FixedArraySlot<T>* slot = array->_.data + pos;
    Ensure(slot->inUse);
    return &slot->element;
}

template<typename T>
FixedArrayAllocation<T> AllocateInFixedArray( FixedArray<T>* array )
{
    FixedArraySlot<T>* slot = NULL;
    int pos;

    // First try to find an unused slot:
    REPEAT(array->_.length, i)
    {
        FixedArraySlot<T>* s = array->_.data + i;
        if(!s->inUse)
        {
            slot = s;
            pos = i;
            break;
        }
    }

    // Extend the array otherwise:
    if(!slot)
    {
        slot = AllocateAtEndOfArray(&array->_, 1);
        pos = array->_.length - 1;
    }

    slot->inUse = true;
    FixedArrayAllocation<T> r = { pos, &slot->element };
    return r;
}

template<typename T>
void RemoveFromFixedArray( FixedArray<T>* array, int pos )
{
    Ensure(pos >= 0 && pos < array->_.length);
    FixedArraySlot<T>* slot = array->_.data + pos;
    Ensure(slot->inUse == true);
    slot->inUse = false;
}

template<typename T>
void CompactFixedArray( FixedArray<T>* array )
{
    int i = array->_.length-1;
    for(; i >= 0; i--)
    {
        const FixedArraySlot<T>* slot = array->_.data + i;
        if(slot->inUse)
        {
            array->_.length = i+1;
            return;
        }
    }
}

#endif
