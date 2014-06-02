#ifndef __TEXTURE__
#define __TEXTURE__

#include "OpenGL.h"
#include "Image.h"


typedef GLuint Texture;

static const int MAX_TEXTURE_UNITS = 8;

static const Texture INVALID_TEXTURE = 0;

enum TextureOptions
{
    TEX_MIPMAP = (1 << 0),
    TEX_FILTER = (1 << 1),
    TEX_CLAMP  = (1 << 2)
};

Texture Create2dTexture( int options, const Image* image );
Texture Load2dTexture( int options, const char* file );

Texture CreateCubeTexture( int options, const Image* images );
Texture LoadCubeTexture( int options, const char* path );

Texture CreateDepthTexture( int width, int height, int options );

void BindTexture( GLenum target, Texture texture, int unit );
void FreeTexture( Texture texture );

#endif
