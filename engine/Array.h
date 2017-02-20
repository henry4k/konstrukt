#ifndef __KONSTRUKT_ARRAY__
#define __KONSTRUKT_ARRAY__

#include <string.h> // memset, memcpy

#include "Common.h" // ReAlloc, Free


template<typename T>
struct Array
{
    int length;
    int capacity;
    T* data;
};


template<typename T>
void InitArray( Array<T>* array )
{
    memset(array, 0, sizeof(Array<T>));
}

template<typename T>
void DestroyArray( Array<T>* array )
{
    Free(array->data);
    InitArray(array);
}

template<typename T>
T* GetArrayElement( Array<T>* array, int pos )
{
    Ensure(pos >= 0 && pos < array->length);
    return array->data + pos;
}

template<typename T>
void ReserveInArray( Array<T>* array, int newCapacity )
{
    Ensure(newCapacity >= 0);
    if(newCapacity > array->capacity)
    {
        array->data = (T*)ReAlloc(array->data, newCapacity*sizeof(T));
        array->capacity = newCapacity;
    }
}

template<typename T>
T* AllocateInArray( Array<T>* array, int pos, int amount )
{
    Ensure(pos >= 0);
    Ensure(pos <= array->length);
    Ensure(amount > 0);

    ReserveInArray(array, array->length+amount);

    T* dst = array->data + (pos + amount);
    T* src = array->data + pos;
    const size_t count = array->length - pos;
    memmove(dst, src, count*sizeof(T));
    array->length += amount;

    return src;
}

template<typename T>
T* AllocateAtEndOfArray( Array<T>* array, int amount )
{
    return AllocateInArray(array, array->length, amount);
}

template<typename T>
T* InsertInArray( Array<T>* array, int pos, int amount, const T* data )
{
    T* r = AllocateInArray(array, pos, amount);
    memcpy(r, data, sizeof(T)*amount);
    return r;
}

template<typename T>
T* AppendToArray( Array<T>* array, int amount, const T* data )
{
    return InsertInArray(array, array->length, amount, data);
}

template<typename T>
void RemoveFromArray( Array<T>* array, int pos, int amount )
{
    Ensure(pos >= 0);
    Ensure(pos < array->length);
    Ensure(amount > 0);
    Ensure(pos+amount <= array->length);

    T* dst = array->data + pos;
    T* src = array->data + pos + amount;
    const size_t count = array->length - pos;
    memmove(dst, src, count*sizeof(T));
    array->length -= amount;
}

template<typename T>
void PopFromArray( Array<T>* array, int amount )
{
    RemoveFromArray(array, array->length-amount, amount);
}

template<typename T>
void ClearArray( Array<T>* array )
{
    array->length = 0;
}

#endif
