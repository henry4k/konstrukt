#ifndef __DUMMY_BDD_HPP__
#define __DUMMY_BDD_HPP__

#include <string>
#include "core.h"

namespace dummy
{

class Describe
{
public:
    Describe( const char* subject ) :
        m_Subject(subject),
        m_Sandbox(NULL)
    {}

    Describe& use( dummySandbox sandbox )
    {
        m_Sandbox = sandbox;
        return *this;
    }

    Describe& it( const char* behaviour, dummySandboxableFunction fn )
    {
        dummyAddTest((m_Subject+" "+behaviour).c_str(), m_Sandbox, fn);
        return *this;
    }

private:
    std::string m_Subject;
    dummySandbox m_Sandbox;
};

}

#endif
