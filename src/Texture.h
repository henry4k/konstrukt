#ifndef __TEXTURE__
#define __TEXTURE__

#include "OpenGL.h"
#include "Image.h"

static const int MaxTextureUnits = 8;

typedef GLuint Texture;

enum TextureOptions
{
    TEX_MIPMAP,
    TEX_FILTER,
    TEX_CLAMP
};

Texture Create2dTexture( int options, const Image* image );
Texture Load2dTexture( int options, const char* file );

Texture CreateCubeTexture( int options, const Image* images );
Texture LoadCubeTexture( int options, const char* path );

Texture CreateDepthTexture( int width, int height, int options );

void BindTexture( GLenum target, Texture texture, int unit );
void FreeTexture( Texture texture );

#endif
