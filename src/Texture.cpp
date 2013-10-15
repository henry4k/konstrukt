#include <string.h>

#include "Common.h"
#include "OpenGL.h"
#include "Image.h"
#include "Texture.h"

GLuint Create2dTexture( int options, const Image* image )
{
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    if(options | TEX_MIPMAP)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (options | TEX_FILTER) ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_NEAREST);
    else
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (options | TEX_FILTER) ? GL_LINEAR : GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (options | TEX_FILTER) ? GL_LINEAR : GL_NEAREST);

    if(options | TEX_MIPMAP)
        glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

    glTexImage2D(GL_TEXTURE_2D, 0, image->format,  image->width, image->height, 0, image->format, image->type, image->data);

    glBindTexture(GL_TEXTURE_2D, 0);

    return texture;
}

GLuint Load2dTexture( int options, const char* file )
{
    Image image;
    if(!LoadImage(&image, file))
        return false;
    const GLuint r = Create2dTexture(options, &image);
    FreeImage(&image);
    return r;
}

GLuint CreateCubeTexture( int options, const Image* images )
{
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    if(options | TEX_MIPMAP)
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, (options | TEX_FILTER) ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_NEAREST);
    else
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, (options | TEX_FILTER) ? GL_LINEAR : GL_NEAREST);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, (options | TEX_FILTER) ? GL_LINEAR : GL_NEAREST);

    if(options | TEX_MIPMAP)
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_GENERATE_MIPMAP, GL_TRUE);

    for(int i = 0; i < 6; i++)
    {
        GLenum target = GL_TEXTURE_CUBE_MAP_POSITIVE_X+i;
        glTexImage2D(target, 0, images[i].format,  images[i].width, images[i].height, 0, images[i].format, images[i].type, images[i].data);
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return texture;
}

GLuint LoadCubeTexture( int options, const char* path )
{
    Image images[6];
    static const char* names[6] = { "Right","Left","Bottom","Top","Front","Back" };
    char buffer[512];
    strncpy(buffer, path, 512);
    int pos = strlen(path);
    for(int i = 0; i < 6; i++)
    {
        if(!LoadImage(&images[i], Format(path, names[i])))
            return 0;
    }
    bool r = CreateCubeTexture(options, images);
    for(int i = 0; i < 6; i++)
    {
        FreeImage(&images[i]);
    }
    return r;
}

void FreeTexture( GLuint texture )
{
    glDeleteTextures(1, &texture);
}

static const int MaxTextureLayers = 8;
GLenum CurTextureTargets[MaxTextureLayers];

int InitTextureLayers()
{
    for(int i = 0; i < 8; i++)
        CurTextureTargets[i] = 0;
    return 0;
}

void BindTexture( GLenum target, const GLuint texture, int layer )
{
    static int unused = InitTextureLayers();
    glActiveTexture(GL_TEXTURE0+layer);
    if(CurTextureTargets[layer] && (CurTextureTargets[layer] != target))
    {
        glBindTexture(CurTextureTargets[layer], 0);
        CurTextureTargets[layer] = target;
    }
    glBindTexture(target, texture);
}
