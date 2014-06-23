#ifndef __APOAPSIS_TEXTURE__
#define __APOAPSIS_TEXTURE__

#include "OpenGL.h"
#include "Image.h"


struct Texture;

enum TextureOptions
{
    TEX_MIPMAP = (1 << 0),
    TEX_FILTER = (1 << 1),
    TEX_CLAMP  = (1 << 2)
};


Texture* Create2dTexture( int options, const Image* image );
Texture* Load2dTexture( int options, const char* filePath );

Texture* CreateCubeTexture( int options, const Image* images );
Texture* LoadCubeTexture( int options, const char* filePath );

Texture* CreateDepthTexture( int width, int height, int options );

void BindTexture( const Texture* texture, int unit );

void ReferenceTexture( Texture* texture );
void ReleaseTexture( Texture* texture );


#endif