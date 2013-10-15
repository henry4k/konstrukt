#ifndef __TEXTURE__
#define __TEXTURE__

#include "OpenGL.h"
#include "Image.h"

enum GLuintOptions
{
    TEX_MIPMAP,
    TEX_FILTER
};

GLuint Create2dTexture( int options, const Image* image );
GLuint Load2dTexture( int options, const char* file );

GLuint CreateCubeTexture( int options, const Image* images );
GLuint LoadCubeTexture( int options, const char* path );

void BindTexture( GLenum target, GLuint texture, int layer );
void FreeTexture( GLuint texture );

#endif
