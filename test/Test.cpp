#include <stdexcept>
#include <dummy/exception_runner.h>
#include "TestTools.h"

void Wrapper()
{
    dummyAbortTest(DUMMY_FAIL_TEST, "Win win win");
}

void Std()
{
    throw std::runtime_error("huhu");
}

void Unknown()
{
    throw 42;
}

int main()
{
    InitTests();
    dummyAddTest("Wrapper", Wrapper);
    dummyAddTest("Std", Std);
    dummyAddTest("Unknown", Unknown);
    return RunTests();
}
