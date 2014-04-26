#include <stdexcept> // logic error
#include "../dummy/core.h"
#include "../dummy/exception_runner.h"

#include "test_reporter.h"


void UnknownExceptionTest()
{
    throw 42;
}

void StdExceptionTest()
{
    throw std::logic_error("This should fail");
}

class Foo
{
public:
    int m_Value;

    Foo( int value ) :
        m_Value(value)
    {
        dummyLog("created Foo with %d", value);
    }

    ~Foo()
    {
        dummyLog("destroyed Foo with %d", m_Value);
    }
};

void PassingDestructorTest()
{
    Foo foo(42);
}

void FailingDestructorTest()
{
    Foo foo(42);
    dummyAbortTest(DUMMY_FAIL_TEST, NULL);
}

int main()
{
    dummyInit(dummyGetExceptionRunner(), dummyGetTestReporter());
    dummyAddTest("UnknownExceptionTest", UnknownExceptionTest);
    dummyAddTest("StdExceptionTest", StdExceptionTest);
    dummyAddTest("PassingDestructorTest", PassingDestructorTest);
    dummyAddTest("FailingDestructorTest", FailingDestructorTest);
    dummyRunTests();
    return 0;
}
