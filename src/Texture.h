#ifndef __TEXTURE__
#define __TEXTURE__

#include "OpenGL.h"
#include "Image.h"

typedef GLuint Texture;

enum TextureOptions
{
    TEX_MIPMAP,
    TEX_FILTER
};

Texture Create2dTexture( int options, const Image* image );
Texture Load2dTexture( int options, const char* file );

Texture CreateCubeTexture( int options, const Image* images );
Texture LoadCubeTexture( int options, const char* path );

void BindTexture( GLenum target, Texture texture, int layer );
void FreeTexture( Texture texture );

#endif
