#include <engine/Math.h>
#include <glm/gtc/matrix_transform.hpp>
#include "TestTools.h"

using namespace glm;

InlineTest("MakeRotationMatrix removes translation.", dummySignalSandbox)
{
    const mat4 input = translate(mat4(1), vec3(1,1,1));
    const mat4 output = MakeRotationMatrix(input);
    const vec4 transformedPoint = output * vec4(0,0,0,1);
    Require(transformedPoint[0] == 0);
    Require(transformedPoint[1] == 0);
    Require(transformedPoint[2] == 0);
    Require(transformedPoint[3] == 1);
}

InlineTest("GetTransformation can copy nothing.", dummySignalSandbox)
{
    const mat4 input = translate(mat4(1), vec3(1,1,1));
    const mat4 output = GetTransformation(input, 0);
    Require(output == mat4(1));
}

InlineTest("GetTransformation can copy only translation.", dummySignalSandbox)
{
    const mat4 translation = translate(mat4(1), vec3(1,1,1));
    const mat4 rotation    = rotate(mat4(1), 90.f, vec3(1,0,0));

    const mat4 output = GetTransformation(translation*rotation, COPY_TRANSLATION);
    Require(output == translation);
}

InlineTest("GetTransformation can copy only rotation.", dummySignalSandbox)
{
    const mat4 translation = translate(mat4(1), vec3(1,1,1));
    const mat4 rotation    = rotate(mat4(1), 90.f, vec3(1,0,0));

    const mat4 output = GetTransformation(translation*rotation, COPY_ROTATION);
    Require(output == rotation);
}

InlineTest("GetTransformation can copy translation and rotation.", dummySignalSandbox)
{
    const mat4 translation = translate(mat4(1), vec3(1,1,1));
    const mat4 rotation    = rotate(mat4(1), 90.f, vec3(1,0,0));

    const mat4 output = GetTransformation(translation*rotation, COPY_TRANSLATION |
                                                                COPY_ROTATION);
    Require(output == translation*rotation);
}

int main( int argc, char** argv )
{
    InitTests(argc, argv);
    return RunTests();
}
