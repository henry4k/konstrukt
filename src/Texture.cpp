#include <string.h>

#include "Common.h"
#include "OpenGL.h"
#include "Image.h"
#include "Texture.h"

void SetTextureOptions( int textureType, int options )
{
    const int wrapMode = (options & TEX_CLAMP) ? GL_CLAMP_TO_EDGE : GL_REPEAT;
    switch(textureType)
    {
        case GL_TEXTURE_CUBE_MAP:
        case GL_TEXTURE_3D:
            glTexParameteri(textureType, GL_TEXTURE_WRAP_R, wrapMode);
        case GL_TEXTURE_2D:
            glTexParameteri(textureType, GL_TEXTURE_WRAP_T, wrapMode);
        case GL_TEXTURE_1D:
            glTexParameteri(textureType, GL_TEXTURE_WRAP_S, wrapMode);
            break;

        default:
            FatalError("Unknown texture type 0x%X", textureType);
    }

    if(options & TEX_MIPMAP)
        glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, (options & TEX_FILTER) ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_NEAREST);
    else
        glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, (options & TEX_FILTER) ? GL_LINEAR : GL_NEAREST);

    glTexParameteri(textureType, GL_TEXTURE_MAG_FILTER, (options & TEX_FILTER) ? GL_LINEAR : GL_NEAREST);

    if(options & TEX_MIPMAP)
        glTexParameteri(textureType, GL_GENERATE_MIPMAP, GL_TRUE);
}

Texture Create2dTexture( int options, const Image* image )
{
    Texture texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    SetTextureOptions(GL_TEXTURE_2D, options);

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
    options |= TEX_CLAMP;

    SetTextureOptions(GL_TEXTURE_CUBE_MAP, options);

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

Texture CreateDepthTexture( int width, int height, int options )
{
    Texture texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    float* data = NULL; // new float[width*height];
    // ^- We don't care about initialization.

    SetTextureOptions(GL_TEXTURE_2D, options);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, data); // TODO: Sure that NULL works here?


    //delete[] data;

    glBindTexture(GL_TEXTURE_2D, 0);

    return texture;
}

void FreeTexture( Texture texture )
{
    glDeleteTextures(1, &texture);
}

GLenum CurTextureTargets[MaxTextureUnits];

int InitTextureUnits()
{
    for(int i = 0; i < 8; i++)
        CurTextureTargets[i] = 0;
    return 0;
}

void BindTexture( GLenum target, const Texture texture, int unit )
{
    static int unused = InitTextureUnits();
    glActiveTexture(GL_TEXTURE0+unit);
    if(CurTextureTargets[unit] && (CurTextureTargets[unit] != target))
    {
        glBindTexture(CurTextureTargets[unit], 0);
        CurTextureTargets[unit] = target;
    }
    glBindTexture(target, texture);
}
