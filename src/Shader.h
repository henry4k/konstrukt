#ifndef __SHADER__
#define __SHADER__

#include "Common.h"

namespace Shader
{
    Handle Load( const char* vert, const char* frag );
    void Free( Handle handle );
    void Bind( Handle handle );
    void SetUniform( Handle shader, const char* name, int value );
    void SetUniform( Handle shader, const char* name, float value );
    void SetUniform( Handle shader, const char* name, int length, float* value );
}

#endif
