#include "../Array.h"
#include "TestTools.h"


struct Position
{
    float x, y, z;
};


void AppendToArrayTest()
{
    Array<Position> list;
    InitArray(&list);

    Require(list.length == 0);
    Require(list.capacity == 0);
    Require(list.data == NULL);

    const Position positions[3] =
    {
        {  1,  2,  3},
        { 10, 20, 30},
        {100,200,300}
    };
    AppendToArray(&list, 3, positions);

    Require(list.length == 3);
    Require(list.capacity >= 3);
    Require(list.data != NULL);
    Require(list.data[0].x == 1);
    Require(list.data[2].z == 300);

    AppendToArray(&list, 3, positions);

    Require(list.length == 6);
    Require(list.capacity >= 6);
    Require(list.data != NULL);
    Require(list.data[0].x == 1);
    Require(list.data[2].z == 300);
    Require(list.data[3].x == 1);
    Require(list.data[5].z == 300);

    DestroyArray(&list);

    Require(list.length == 0);
    Require(list.capacity == 0);
    Require(list.data == NULL);
}

void InsertInArrayTest()
{
    Array<Position> list;
    InitArray(&list);

    const Position positions[3] =
    {
        {  1,  2,  3},
        { 10, 20, 30},
        {100,200,300}
    };
    AppendToArray(&list, 3, positions);

    const Position otherPositions[2] =
    {
        {111,222,333},
        {444,555,666},
    };

    InsertInArray(&list, 1, 2, otherPositions);

    Require(list.length == 5);
    Require(list.capacity >= 5);
    Require(list.data[0].z == 3);
    Require(list.data[1].x == 111);
    Require(list.data[2].z == 666);
    Require(list.data[3].x == 10);
    Require(list.data[4].z == 300);

    DestroyArray(&list);
}

void RemoveFromArrayTest()
{
    Array<Position> list;
    InitArray(&list);

    const Position positions[4] =
    {
        {   1,   2,   3},
        {  10,  20,  30},
        { 100, 200, 300},
        {1000,2000,3000}
    };
    AppendToArray(&list, 4, positions);

    RemoveFromArray(&list, 1, 2);

    Require(list.length == 2);
    Require(list.data[0].x == 1);
    Require(list.data[0].z == 3);
    Require(list.data[1].x == 1000);
    Require(list.data[1].z == 3000);

    DestroyArray(&list);
}


int main( int argc, char** argv )
{
    InitTests(argc, argv);

    AddTest("AppendToArray", AppendToArrayTest);
    AddTest("InsertInArray", InsertInArrayTest);
    AddTest("RemoveFromArrayTest", RemoveFromArrayTest);

    return RunTests();
}
