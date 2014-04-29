#ifndef __DUMMY_BDD_HPP__
#define __DUMMY_BDD_HPP__

#include <string>
#include "core.h"

namespace dummy
{

class Describe
{
public:
    Describe( const char* subject ) : m_Subject(subject) {}

    Describe& it( const char* behaviour, dummyTestFunction fn )
    {
        dummyAddTest((m_Subject+" "+behaviour).c_str(), fn);
        return *this;
    }

private:
    std::string m_Subject;
};

}

#endif
