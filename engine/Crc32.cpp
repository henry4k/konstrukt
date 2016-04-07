#include <assert.h>
#include <string.h> // strlen

#include "Crc32.h"


//const uint32_t Crc32Poly = 0x04C11DB7;
const uint32_t Crc32PolyRev = 0xEDB88320;
static uint32_t Crc32Table[256];


void InitCrc32()
{
    uint32_t i, j, h = 1;
    Crc32Table[0] = 0;
    for(i = 128; i; i >>= 1)
    {
        h = (h >> 1) ^ ((h & 1) ? Crc32PolyRev : 0);
        // h is now Crc32Table[i]
        for(j = 0; j < 256; j += 2 * i)
            Crc32Table[i + j] = Crc32Table[j] ^ h;
    }

    assert(Crc32Table[255] != 0);
}

/*
 * This computes the standard preset and inverted CRC, as used
 * by most networking standards.  Start by passing in an initial
 * chaining value of 0, and then pass in the return value from the
 * previous crc32() call.  The final return value is the CRC.
 * Note that this is a little-endian CRC, which is best used with
 * data transmitted lsbit-first, and it should, itself, be appended
 * to data in little-endian byte and bit order to preserve the
 * property of detecting all burst errors of length 32 bits or less.
 */
static uint32_t CalcCrc32( uint32_t crc, const char* buffer, int length )
{
    InitCrc32();
    crc ^= 0xFFFFFFFF;
    while(length--)
        crc = (crc >> 8) ^ Crc32Table[(crc ^ *buffer++) & 0xFF];
    return crc ^ 0xFFFFFFFF;
}

uint32_t CalcCrc32ForBuffer( const void* buffer, int length )
{
    uint32_t crc;
    crc = CalcCrc32(0L, 0, 0);
    crc = CalcCrc32(crc, (char*)buffer, length);
    return crc;
}

uint32_t CalcCrc32ForString( const char* string )
{
    return CalcCrc32ForBuffer(string, strlen(string));
}
