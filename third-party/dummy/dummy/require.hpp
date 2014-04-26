#ifndef __DUMMY_REQUIRE_HPP__
#define __DUMMY_REQUIRE_HPP__

namespace dummy
{

// bool, long, double, string, void*


class Requirement
{
public:
    Requirement( const char* expression );

protected:
    const char* m_Expression;
};


class BoolRequirement : public Requirement
{
public:
    BoolRequirement( const char* expression, bool value );
    BoolRequirement& is( bool value );
    BoolRequirement& isNot( bool value );
    BoolRequirement& equals( bool value );

private:
    bool m_Value;
};

class IntRequirement : public Requirement
{
public:
    IntRequirement( const char* expression, long value );
    IntRequirement& is( long value );
    IntRequirement& isNot( long value );
    IntRequirement& equals( long value );
    IntRequirement& isGreaterThan( long value );
    IntRequirement& isGreaterOrEqualTo( long value );
    IntRequirement& isSmallerThan( long value );
    IntRequirement& isSmallerOrEqualTo( long value );

private:
    long m_Value;
};

class FloatRequirement : public Requirement
{
public:
    FloatRequirement( const char* expression, double value );
    FloatRequirement& withEpsilon( double epsilon );
    FloatRequirement& is( double value );
    FloatRequirement& isNot( double value );
    FloatRequirement& equals( double value );
    FloatRequirement& isGreaterThan( double value );
    FloatRequirement& isGreaterOrEqualTo( double value );
    FloatRequirement& isSmallerThan( double value );
    FloatRequirement& isSmallerOrEqualTo( double value );

private:
    double m_Value;
    double m_Epsilon;
};

class StringRequirement : public Requirement
{
public:
    StringRequirement( const char* expression, const char* value );
    StringRequirement& is( const char* value );
    StringRequirement& isNot( const char* value );
    StringRequirement& equals( const char* value );
    StringRequirement& beginsWith( const char* value );
    StringRequirement& endsWith( const char* value );
    StringRequirement& contains( const char* value );
    StringRequirement& matches( const char* value );

private:
    const char* m_Value;
};

class PointerRequirement : public Requirement
{
public:
    StringRequirement( const char* expression, const void* value );
    StringRequirement& is( const void* value );
    StringRequirement& isNot( const void* value );
    StringRequirement& equals( const void* value );

private:
    const void* m_Value;
};


#define RequireThat( E ) RequireThat_(#E, E)
BoolRequirement RequireThat_( const char* expression, bool value );
IntRequirement RequireThat_( const char* expression, long value );
FloatRequirement RequireThat_( const char* expression, double value );
StringRequirement RequireThat_( const char* expression, const char* value );
PointerRequirement RequireThat_( const char* expression, const void* value );


// ---- bool implementation ----

BoolRequirement::BoolRequirement( const char* expression, bool value ) :
    Requirement(expression),
    m_Value(value)
{
}

BoolRequirement& BoolRequirement::is( bool value )
{
    const char* expected = (value == true) ? "true" : "false";
    const char* got = (m_Value == true) ? "true" : "false";

    if(m_Value != value)
        dummyAbortTest(DUMMY_FAIL_TEST, "Expected %s to be %s, but it was %s.",
            m_Expression,
            expected,
            got);
    return *this;
}

BoolRequirement& BoolRequirement::isNot( bool value )
{
    const char* expected = (value == true) ? "true" : "false";
    const char* got = (m_Value == true) ? "true" : "false";

    if(m_Value == value)
        dummyAbortTest(DUMMY_FAIL_TEST, "Expected %s to be %s, but it was %s.",
            m_Expression,
            expected,
            got);
    return *this;
}

BoolRequirement& BoolRequirement::equals( bool value )
{
    return is(value);
}

BoolRequirement RequireThat_( const char* expression, bool value )
{
    return BoolRequirement(expression, value);
}


// ---- int implementation ----

IntRequirement::IntRequirement( const char* expression, long value ) :
    Requirement(expression),
    m_Value(value)
{
}

IntRequirement( const char* expression, long value )
{

}

IntRequirement& IntRequirement::is( long value )
{
}

IntRequirement& IntRequirement::isNot( long value )
{
}

IntRequirement& IntRequirement::equals( long value )
{
}

IntRequirement& IntRequirement::isGreaterThan( long value )
{
}

IntRequirement& IntRequirement::isGreaterOrEqualTo( long value )
{
}

IntRequirement& IntRequirement::isSmallerThan( long value )
{
}

IntRequirement& IntRequirement::isSmallerOrEqualTo( long value )
{
}


}

#endif
