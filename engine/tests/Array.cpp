#include "../Array.h"
#include "TestTools.h"


InlineTest("InitArray")
{
    Array<int> list;
    InitArray(&list);

    Require(list.length == 0);
    Require(list.capacity == 0);
    Require(list.data == NULL);
}

InlineTest("ReserveInArray")
{
    Array<int> list;
    InitArray(&list);

    ReserveInArray(&list, 3);

    Require(list.length == 0);
    Require(list.capacity >= 3);
    Require(list.data != NULL);

    const int oldCapacity = list.capacity;
    const int* oldData = list.data;

    ReserveInArray(&list, 3); // reserving the same amount twice should be a no-op

    Require(list.length == 0);
    Require(list.capacity == oldCapacity);
    Require(list.data == oldData);

    ReserveInArray(&list, 10);

    Require(list.length == 0);
    Require(list.capacity >= 10);
    Require(list.data != NULL);

    DestroyArray(&list);
}

InlineTest("AllocateInArray")
{
    Array<int> list;
    InitArray(&list);

    AllocateInArray(&list, 0, 0); // allocate nothing

    Require(list.length == 0);
    Require(list.capacity == 0);
    Require(list.data == NULL);

    int* allocation = AllocateInArray(&list, 0, 3);

    Require(list.length == 3);
    Require(list.capacity >= 3);
    Require(list.data != NULL);

    allocation[0] = 1;
    allocation[1] = 2;
    allocation[2] = 3;

    Require(list.data[0] == 1);
    Require(list.data[1] == 2);
    Require(list.data[2] == 3);

    const int oldCapacity = list.capacity;
    const int* oldData = list.data;

    AllocateInArray(&list, 2, 0); // allocate nothing

    Require(list.length == 3);
    Require(list.capacity == oldCapacity);
    Require(list.data == oldData);

    Require(list.data[0] == 1);
    Require(list.data[1] == 2);
    Require(list.data[2] == 3);

    allocation = AllocateInArray(&list, 1, 3);

    Require(list.length == 6);
    Require(list.capacity >= 6);
    Require(list.data != NULL);

    allocation[0] = 11;
    allocation[1] = 22;
    allocation[2] = 33;

    Require(list.data[0] == 1);
    Require(list.data[1] == 11);
    Require(list.data[2] == 22);
    Require(list.data[3] == 33);
    Require(list.data[4] == 2);
    Require(list.data[5] == 3);

    DestroyArray(&list);
}

InlineTest("AllocateAtEndOfArray")
{
    Array<int> list;
    InitArray(&list);

    AllocateAtEndOfArray(&list, 0); // allocate nothing

    Require(list.length == 0);
    Require(list.capacity == 0);
    Require(list.data == NULL);

    int* allocation = AllocateAtEndOfArray(&list, 3);

    Require(list.length == 3);
    Require(list.capacity >= 3);
    Require(list.data != NULL);

    allocation[0] = 1;
    allocation[1] = 2;
    allocation[2] = 3;

    allocation = AllocateAtEndOfArray(&list, 3);

    Require(list.length == 6);
    Require(list.capacity >= 6);
    Require(list.data != NULL);

    allocation[0] = 4;
    allocation[1] = 5;
    allocation[2] = 6;

    Require(list.data[0] == 1);
    Require(list.data[1] == 2);
    Require(list.data[2] == 3);
    Require(list.data[3] == 4);
    Require(list.data[4] == 5);
    Require(list.data[5] == 6);

    DestroyArray(&list);
}

InlineTest("AppendToArray")
{
    Array<int> list;
    InitArray(&list);

    const int numbers[] = {1, 2, 3};
    AppendToArray(&list, 3, numbers);

    Require(list.length == 3);
    Require(list.capacity >= 3);
    Require(list.data != NULL);
    Require(list.data[0] == 1);
    Require(list.data[2] == 3);

    AppendToArray(&list, 3, numbers);

    Require(list.length == 6);
    Require(list.capacity >= 6);
    Require(list.data != NULL);
    Require(list.data[0] == 1);
    Require(list.data[2] == 3);
    Require(list.data[3] == 1);
    Require(list.data[5] == 3);

    DestroyArray(&list);
}

InlineTest("AppendToArray (empty source)")
{
    Array<int> list;
    InitArray(&list);

    int fake;
    AppendToArray(&list, 0, &fake); // an empty list

    Require(list.length == 0);
    Require(list.capacity == 0);
    Require(list.data == NULL);

    AppendToArray(&list, 0, (const int*)NULL); // no list

    Require(list.length == 0);
    Require(list.capacity == 0);
    Require(list.data == NULL);

    DestroyArray(&list);
}

InlineTest("InsertInArray")
{
    Array<int> list;
    InitArray(&list);

    const int numbers[] = {1, 2, 3};
    AppendToArray(&list, 3, numbers);

    const int otherNumbers[] = {111, 222};
    InsertInArray(&list, 1, 2, otherNumbers);

    Require(list.length == 5);
    Require(list.capacity >= 5);
    Require(list.data[0] == 1);
    Require(list.data[1] == 111);
    Require(list.data[2] == 222);
    Require(list.data[3] == 2);
    Require(list.data[4] == 3);

    DestroyArray(&list);
}

InlineTest("RemoveFromArray")
{
    Array<int> list;
    InitArray(&list);

    const int numbers[] = {1, 2, 3, 4};
    AppendToArray(&list, 4, numbers);

    RemoveFromArray(&list, 1, 2);

    Require(list.length == 2);
    Require(list.data[0] == 1);
    Require(list.data[1] == 4);

    DestroyArray(&list);
}

InlineTest("PopFromArray")
{
    Array<int> list;
    InitArray(&list);

    const int numbers[] = {1, 2, 3};
    AppendToArray(&list, 3, numbers);

    PopFromArray(&list, 2);

    Require(list.length == 1);
    Require(list.data[0] == 1);

    DestroyArray(&list);
}

InlineTest("ClearArray")
{
    Array<int> list;
    InitArray(&list);

    const int numbers[] = {1, 2, 3};
    AppendToArray(&list, 3, numbers);

    ClearArray(&list);

    Require(list.length == 0);
    Require(list.capacity >= 0);
    Require(list.data != NULL);

    DestroyArray(&list);
}

int main( int argc, char** argv )
{
    InitTests(argc, argv);
    return RunTests();
}
