#ifndef __KONSTRUKT_TEXTURE__
#define __KONSTRUKT_TEXTURE__

#include "OpenGL.h"


struct Image;
struct Texture;

static const int MAX_TEXTURE_UNITS = 8;

enum TextureOptions
{
    TEX_MIPMAP = (1 << 0),
    TEX_FILTER = (1 << 1),
    TEX_CLAMP  = (1 << 2),
    TEX_SRGB   = (1 << 3)
};


Texture* Create2dTexture( const Image* image, int options );
Texture* CreateCubeTexture( const Image** images, int options );
Texture* CreateDepthTexture( int width, int height, int options );

void BindTexture( const Texture* texture, int unit );

void ReferenceTexture( Texture* texture );
void ReleaseTexture( Texture* texture );


#endif
