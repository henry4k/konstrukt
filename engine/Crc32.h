#ifndef __KONSTRUKT_CRC32__
#define __KONSTRUKT_CRC32__

#include <stdint.h>

void InitCrc32();
uint32_t CalcCrc32ForBuffer( const void* buffer, int length );
uint32_t CalcCrc32ForString( const char* string );

#endif
