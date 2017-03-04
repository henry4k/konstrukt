#ifndef __KONSTRUKT_OBJECT_SYSTEM__
#define __KONSTRUKT_OBJECT_SYSTEM__

#include <assert.h>
#include <stddef.h> // NULL
#include <stdint.h> // uint16_t, uint32_t, ...
#include <string.h> // memset

#include "Common.h" // static_assert
#include "Array.h"


// This is an implementation of the ID lookup table, which is described here:
// https://bitsquid.blogspot.de/2011/09/managing-decoupling-part-4-id-lookup.html


/**
 * An object ID consists of two parts:
 *
 * - an index, which refers to a #ObjectReference
 * - a more or less unique value, which prevents that the ID of a removed
 *   objects can be used to a access a newly created one.
 *
 * #OBJECT_INDEX_BITS determines how many bits of #ObjectId are used
 * for the index part.
 */
typedef uint32_t ObjectId;

/**
 * Index part of the #ObjectId.
 *
 * See #OBJECT_INDEX_BITS.
 */
typedef uint16_t ObjectIndex;

static const int OBJECT_INDEX_BITS = 12; // See #ObjectIndex
static_assert(OBJECT_INDEX_BITS <= sizeof(ObjectIndex)*8, "");
static_assert(OBJECT_INDEX_BITS < sizeof(ObjectId)*8, "");

static const int OBJECT_REFERENCE_COUNT = 1 << OBJECT_INDEX_BITS;
static const int MAX_OBJECTS            = (1 << OBJECT_INDEX_BITS) - 1; // for use in tests
static const ObjectId OBJECT_INDEX_MASK = (1 << OBJECT_INDEX_BITS) - 1;
static const ObjectId OBJECT_ID_UNIQUE_INCREMENT = OBJECT_INDEX_MASK + 1;
static const ObjectIndex INVALID_OBJECT_INDEX = UINT16_MAX;


/**
 * Maps IDs to object indices and sim maintains a list of free IDs.
 */
struct ObjectReference
{
    ObjectId id;

    /**
     * Links to the next free list entry.
     * See #ObjectSystem.
     */
    ObjectIndex nextFreeRefIndex;

    /**
     * Refers to an entry in the object array or must be set to
     * #INVALID_OBJECT_INDEX.
     */
    ObjectIndex objectIndex;
};

/**
 * Just a small wrapper for objects, as we need to store their ID with them.
 */
template<typename T>
struct ObjectSlot
{
    ObjectId id;
    T object;
};

/**
 *
 * An object system manages objects, by managing the required memory and
 * providing unique IDs.
 *
 * IDs are neither pointers nor direct array references, which point to an
 * object.  They rather carry an index to a look-up table.  This table holds
 * the actual object index.  This allows the sys to optimize the objects
 * memory.
 */
template<typename T>
struct ObjectSystem
{
    /**
     * Start marker of the free list - a double ended queue.
     */
    ObjectIndex firstFreeRefIndex;

    /**
     * End marker of the free list.
     * See #firstFreeRefIndex.
     */
    ObjectIndex lastFreeRefIndex;

    /**
     * Maps IDs, without the unique part, to object references.
     * Use #GetReferenceIndex to strip the unique part.
     * Has `2^n` entries, where n is #OBJECT_INDEX_BITS.
     */
    ObjectReference* references;

    Array<ObjectSlot<T>> objects;
};

template<typename T>
void InitObjectSystem( ObjectSystem<T>* sys )
{

    sys->references =
        (ObjectReference*)Alloc(sizeof(ObjectReference)*OBJECT_REFERENCE_COUNT);

    REPEAT(OBJECT_REFERENCE_COUNT, i)
    {
        ObjectReference* ref = sys->references + i;
        ref->id = i;
        ref->nextFreeRefIndex = i+1;
        ref->objectIndex = INVALID_OBJECT_INDEX;
    }

    sys->firstFreeRefIndex = 0;
    sys->lastFreeRefIndex  = OBJECT_REFERENCE_COUNT-1;

    InitArray(&sys->objects);
}

template<typename T>
void DestroyObjectSystem( ObjectSystem<T>* sys )
{
    Ensure(sys->objects.length == 0); // all objects should have been removed
    Free(sys->references);
    DestroyArray(&sys->objects);
}

/**
 * Strips the unique ID part.
 */
inline ObjectIndex GetReferenceIndex( ObjectId id )
{
    return id & OBJECT_INDEX_MASK;
}

template<typename T>
ObjectReference* TryGetObjectReference( ObjectSystem<T>* sys, ObjectId id )
{
    // No need to check whether index is in range, as all possible values are
    // valid.  See #InitObjectSystem and #OBJECT_INDEX_MASK.
    ObjectReference* ref = sys->references + GetReferenceIndex(id);
    if(ref->id == id && ref->objectIndex != INVALID_OBJECT_INDEX)
        return ref;
    else
        return NULL;
}

template<typename T>
bool HasObject( ObjectSystem<T>* sys, ObjectId id )
{
    return TryGetObjectReference(sys, id) != NULL;
}

template<typename T>
T* GetObject( ObjectSystem<T>* sys, ObjectId id )
{
    const ObjectReference* ref = TryGetObjectReference(sys, id);
    Ensure(ref);

    assert(ref->objectIndex != INVALID_OBJECT_INDEX);
    assert(ref->objectIndex >= 0 &&
           ref->objectIndex < sys->objects.length);
    ObjectSlot<T>* slot = sys->objects.data + ref->objectIndex;
    assert(slot->id == id);

    return &slot->object;
}

template<typename T>
ObjectId AllocateObject( ObjectSystem<T>* sys )
{
    if(sys->firstFreeRefIndex == sys->lastFreeRefIndex)
        FatalError("Can't allocate more objects.");

    ObjectReference* ref = sys->references + sys->firstFreeRefIndex;
    sys->firstFreeRefIndex = ref->nextFreeRefIndex;

    ref->id += OBJECT_ID_UNIQUE_INCREMENT; // increment the unique part

    ref->objectIndex = sys->objects.length;
    ObjectSlot<T>* slot = AllocateAtEndOfArray(&sys->objects, 1);
    slot->id = ref->id;
    return ref->id;
}

template<typename T>
void RemoveObject( ObjectSystem<T>* sys, ObjectId id )
{
    ObjectReference* ref = TryGetObjectReference(sys, id);
    Ensure(ref);

    // Move the last object to this slot:
    ObjectSlot<T>* object = sys->objects.data + ref->objectIndex;
                  *object = sys->objects.data[sys->objects.length - 1];

    // Adapt reference of last object:
    ObjectReference* replacedRef = TryGetObjectReference(sys, object->id);
    Ensure(replacedRef);
    replacedRef->objectIndex = ref->objectIndex;

    // Reduce object array by one:
    PopFromArray(&sys->objects, 1);

    // Invalidate removed objects reference:
    ref->objectIndex = INVALID_OBJECT_INDEX;

    // Add it to the free list:
    const ObjectIndex index = GetReferenceIndex(id);
    sys->references[sys->lastFreeRefIndex].nextFreeRefIndex = index;
    sys->lastFreeRefIndex = index;
}

template<typename T>
void ReserveObjects( ObjectSystem<T>* sys, int capacity )
{
    assert(capacity <= OBJECT_REFERENCE_COUNT);
    ReserveInArray(&sys->objects, capacity);
}

template<typename T>
T* GetObjectByIndex( ObjectSystem<T>* sys, ObjectIndex index )
{
    return &GetArrayElement(&sys->objects, index)->object;
}

template<typename T>
ObjectId GetObjectIdByIndex( ObjectSystem<T>* sys, ObjectIndex index )
{
    return GetArrayElement(&sys->objects, index)->id;
}

template<typename T>
int GetObjectCount( ObjectSystem<T>* sys )
{
    return sys->objects.length;
}


#endif
