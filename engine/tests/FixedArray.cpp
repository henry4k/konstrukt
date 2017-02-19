#include "../FixedArray.h"
#include "TestTools.h"


struct Job
{
    char name;
};


void Test()
{
    FixedArray<Job> jobs;
    InitFixedArray(&jobs);

    int jobAPos;
    int jobBPos;
    int jobCPos;

    {
        FixedArrayAllocation<Job> allocation = AllocateInFixedArray(&jobs);
        allocation.element->name = 'A';
        jobAPos = allocation.pos;
    }
    Require(GetFixedArrayElement(&jobs, jobAPos)->name == 'A');

    {
        FixedArrayAllocation<Job> allocation = AllocateInFixedArray(&jobs);
        allocation.element->name = 'B';
        jobBPos = allocation.pos;
    }
    Require(GetFixedArrayElement(&jobs, jobAPos)->name == 'A');
    Require(GetFixedArrayElement(&jobs, jobBPos)->name == 'B');

    RemoveFromFixedArray(&jobs, jobAPos);
    Require(GetFixedArrayElement(&jobs, jobBPos)->name == 'B');

    {
        FixedArrayAllocation<Job> allocation = AllocateInFixedArray(&jobs);
        allocation.element->name = 'C';
        jobCPos = allocation.pos;
    }
    Require(GetFixedArrayElement(&jobs, jobBPos)->name == 'B');
    Require(GetFixedArrayElement(&jobs, jobCPos)->name == 'C');

    DestroyFixedArray(&jobs);
}


int main( int argc, char** argv )
{
    InitTests(argc, argv);

    AddTest("Test", Test);

    return RunTests();
}
