#ifndef __APOAPSIS_TEXTURE__
#define __APOAPSIS_TEXTURE__

#include "OpenGL.h"


struct Image;
struct Texture;

static const int MAX_TEXTURE_UNITS = 8;

enum TextureOptions
{
    TEX_MIPMAP    = (1 << 0),
    TEX_FILTER    = (1 << 1),
    TEX_CLAMP     = (1 << 2)
};


Texture* Load2dTexture( int options, const char* vfsPath );
Texture* LoadCubeTexture( int options, const char* vfsPathTemplate );

void BindTexture( const Texture* texture, int unit );

void ReferenceTexture( Texture* texture );
void ReleaseTexture( Texture* texture );


#endif
