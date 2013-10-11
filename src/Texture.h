#ifndef __TEXTURE__
#define __TEXTURE__

#include "Common.h"

enum TextureOptions
{
    TEX_MIPMAP,
    TEX_FILTER
};

struct Texture
{
    Handle name;
    int type;
};

bool Create2dTexture( Texture* texture, int options, const Image* image );
bool Load2dTexture( Texture* texture, int options, const char* file );

bool CreateCubeTexture( Texture* texture, int options, const Image* images );
bool LoadCubeTexture( Texture* texture, int options, const char* path, const char* extension ); // "Foo/Right.png" usw.

void BindTexture( const Texture* texture, int layer );
void FreeTexture( const Texture* texture );

#endif
