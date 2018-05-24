#include "../Crc32.h"
#include "TestTools.h"


InlineTest("CalcCrc32For*")
{
    InitCrc32();
    const char buffer[] = "foobar";
    const uint32_t checksum = 0x9EF61F95;
    Require(CalcCrc32ForBuffer(buffer, 6) == checksum);
    Require(CalcCrc32ForString(buffer)    == checksum);
}

int main( int argc, char** argv )
{
    InitTests(argc, argv);
    return RunTests();
}
