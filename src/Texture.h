#ifndef __TEXTURE__
#define __TEXTURE__

#include "Common.h"

struct Texture
{
    enum TextureOptions
    {
        TEX_MIPMAP,
        TEX_FILTER
    };

    Handle name;
    int type;

    static bool Create2d( Texture* texture, int options, const Image* image );
    static bool Load2d( Texture* texture, int options, const char* file );

    static bool CreateCube( Texture* texture, int options, const Image* images );
    static bool LoadCube( Texture* texture, int options, const char* path, const char* extension ); // "Foo/Right.png" usw.

    static void Bind( const Texture* texture, int layer );
    static void Free( const Texture* texture );
};


#endif
