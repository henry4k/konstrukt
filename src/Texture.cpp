#include <string.h>

#include "Common.h"
#include "OpenGL.h"
#include "Image.h"
#include "Texture.h"

Texture Create2dTexture( int options, const Image* image )
{
    Texture texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    const int wrapMode = (options & TEX_CLAMP) ? GL_CLAMP_TO_EDGE : GL_REPEAT;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);

    if(options & TEX_MIPMAP)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (options & TEX_FILTER) ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_NEAREST);
    else
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (options & TEX_FILTER) ? GL_LINEAR : GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (options & TEX_FILTER) ? GL_LINEAR : GL_NEAREST);

    if(options & TEX_MIPMAP)
        glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

    glTexImage2D(GL_TEXTURE_2D, 0, image->format,  image->width, image->height, 0, image->format, image->type, image->data);

    glBindTexture(GL_TEXTURE_2D, 0);

    return texture;
}

Texture Load2dTexture( int options, const char* file )
{
    Image image;
    if(!LoadImage(&image, file))
        return false;
    const Texture r = Create2dTexture(options, &image);
    FreeImage(&image);
    if(r > 0)
        Log("Loaded %s", file);
    else
        Error("Failed to load %s", file);
    return r;
}

Texture CreateCubeTexture( int options, const Image* images )
{
    Texture texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

    // Always uses clamp to edge since its the only option that makes sense here.
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    if(options & TEX_MIPMAP)
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, (options & TEX_FILTER) ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_NEAREST);
    else
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, (options & TEX_FILTER) ? GL_LINEAR : GL_NEAREST);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, (options & TEX_FILTER) ? GL_LINEAR : GL_NEAREST);

    if(options & TEX_MIPMAP)
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_GENERATE_MIPMAP, GL_TRUE);

    for(int i = 0; i < 6; i++)
    {
        GLenum target = GL_TEXTURE_CUBE_MAP_POSITIVE_X+i;
        glTexImage2D(target, 0, images[i].format,  images[i].width, images[i].height, 0, images[i].format, images[i].type, images[i].data);
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return texture;
}

Texture LoadCubeTexture( int options, const char* path )
{
    Image images[6];
    static const char* names[6] = { "px","nx","ny","py","pz","nz" };
    char buffer[512];
    strncpy(buffer, path, 512);
    int pos = strlen(path);
    for(int i = 0; i < 6; i++)
    {
        if(!LoadImage(&images[i], Format(path, names[i])))
        {
            Error("Failed to load %s", Format(path, names[i]));
            return 0;
        }
    }
    const Texture r = CreateCubeTexture(options, images);
    for(int i = 0; i < 6; i++)
    {
        FreeImage(&images[i]);
    }
    if(r > 0)
        Log("Loaded %s", Format(path, "*"));
    else
        Error("Failed to load %s", Format(path, "*"));
    return r;
}

Texture CreateDepthTexture( int width, int height )
{
    Texture texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    float* data = NULL; // new float[width*height];
    // ^- We don't care about initialization.

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, data); // TODO: Sure that NULL works here?

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    //delete[] data;

    glBindTexture(GL_TEXTURE_2D, 0);

    return texture;
}

void FreeTexture( Texture texture )
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

void BindTexture( GLenum target, const Texture texture, int layer )
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
