#include <stdlib.h> // NULL
#include <float.h> // FLT_MAX

#include "Common.h"
#include "Config.h"
#include "OpenGL.h"
#include "Image.h"
#include "Reference.h"
#include "Texture.h"


static const GLuint INVALID_TEXTURE_HANDLE = 0;

static const int MAX_CHANNEL_COUNT = 4;

static const int ChannelCountToFormat[MAX_CHANNEL_COUNT] =
{
    GL_LUMINANCE,
    GL_LUMINANCE_ALPHA,
    GL_RGB,
    GL_RGBA
};

static const int ChannelCountToSRGBFormat[MAX_CHANNEL_COUNT] =
{
    GL_SLUMINANCE,
    GL_SLUMINANCE_ALPHA,
    GL_SRGB,
    GL_SRGB_ALPHA
};


struct Texture
{
    ReferenceCounter refCounter;
    GLenum target;
    GLuint handle;
};


static int GetImageFormat( int channelCount, bool useSRGB )
{
    if(channelCount < 1 || channelCount > MAX_CHANNEL_COUNT)
    {
        FatalError("Unsupported channel count: %d", channelCount);
        return 0;
    }

    if(useSRGB)
        return ChannelCountToSRGBFormat[channelCount-1];
    else
        return ChannelCountToFormat[channelCount-1];
}

static float GetMaxAnisotropy()
{
    if(FLEXT_EXT_texture_filter_anisotropic)
    {
        const float user = GetConfigFloat("opengl.anisotropy", FLT_MAX);

        float limit = 0;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &limit);

        return user>limit ? limit : user;
    }
    else
    {
        return 0;
    }
}

static void SetTextureOptions( int target, int options )
{
    const int wrapMode = (options & TEX_CLAMP) ? GL_CLAMP_TO_EDGE : GL_REPEAT;
    switch(target)
    {
        case GL_TEXTURE_CUBE_MAP:
        case GL_TEXTURE_3D:
            glTexParameteri(target, GL_TEXTURE_WRAP_R, wrapMode);
        case GL_TEXTURE_2D:
            glTexParameteri(target, GL_TEXTURE_WRAP_T, wrapMode);
        case GL_TEXTURE_1D:
            glTexParameteri(target, GL_TEXTURE_WRAP_S, wrapMode);
            break;

        default:
            FatalError("Unknown texture target 0x%X", target);
    }

    if(options & TEX_MIPMAP)
        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, (options & TEX_FILTER) ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_NEAREST);
    else
        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, (options & TEX_FILTER) ? GL_LINEAR : GL_NEAREST);

    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, (options & TEX_FILTER) ? GL_LINEAR : GL_NEAREST);

    if(options & TEX_MIPMAP)
        glTexParameteri(target, GL_GENERATE_MIPMAP, GL_TRUE);

    const float maxAnisotropy = GetMaxAnisotropy();
    if(maxAnisotropy > 0)
        glTexParameterf(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);
}

static Texture* CreateTexture( GLenum target, int options )
{
    Texture* texture = new Texture;
    InitReferenceCounter(&texture->refCounter);
    texture->target = target;
    glGenTextures(1, &texture->handle);

    glBindTexture(target, texture->handle);
    SetTextureOptions(target, options);
    glBindTexture(target, INVALID_TEXTURE_HANDLE);

    return texture;
}

Texture* Create2dTexture( const Image* image, int options )
{
    Texture* texture = CreateTexture(GL_TEXTURE_2D, options);
    if(!texture)
        return NULL;

    glBindTexture(GL_TEXTURE_2D, texture->handle);
    const int internalFormat = GetImageFormat(image->channelCount, options & TEX_SRGB);
    const int format         = GetImageFormat(image->channelCount, false);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 internalFormat,
                 image->width,
                 image->height,
                 0,
                 format,
                 image->type,
                 image->data);
    glBindTexture(GL_TEXTURE_2D, INVALID_TEXTURE_HANDLE);
    return texture;
}

Texture* CreateCubeTexture( const Image** images, int options )
{
    // Always uses clamp to edge since its the only option that makes sense here.
    options |= TEX_CLAMP;

    Texture* texture = CreateTexture(GL_TEXTURE_CUBE_MAP, options);
    if(!texture)
        return NULL;

    glBindTexture(GL_TEXTURE_CUBE_MAP, texture->handle);
    for(int i = 0; i < 6; i++)
    {
        const Image* image = images[i];
        const GLenum target = GL_TEXTURE_CUBE_MAP_POSITIVE_X+i;
        const int internalFormat = GetImageFormat(image->channelCount, options & TEX_SRGB);
        const int format         = GetImageFormat(image->channelCount, false);
        glTexImage2D(target,
                     0,
                     internalFormat,
                     image->width,
                     image->height,
                     0,
                     format,
                     image->type,
                     image->data);
    }
    glBindTexture(GL_TEXTURE_CUBE_MAP, INVALID_TEXTURE_HANDLE);

    return texture;
}

Texture* CreateDepthTexture( int width, int height, int options )
{
    Texture* texture = CreateTexture(GL_TEXTURE_2D, options);
    if(!texture)
        return NULL;

    glBindTexture(GL_TEXTURE_2D, texture->handle);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
    const float* data = NULL; // TODO: Sure that NULL works here?
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_DEPTH_COMPONENT16,
                 width,
                 height,
                 0,
                 GL_DEPTH_COMPONENT,
                 GL_FLOAT,
                 data);
    glBindTexture(GL_TEXTURE_2D, INVALID_TEXTURE_HANDLE);
    return texture;
}

static GLenum CurTextureTargets[MAX_TEXTURE_UNITS] =
{
    INVALID_TEXTURE_HANDLE,
    INVALID_TEXTURE_HANDLE,
    INVALID_TEXTURE_HANDLE,
    INVALID_TEXTURE_HANDLE
};

void BindTexture( const Texture* texture, int unit )
{
    glActiveTexture(GL_TEXTURE0+unit);
    if(CurTextureTargets[unit] && (CurTextureTargets[unit] != texture->target))
    {
        glBindTexture(CurTextureTargets[unit], INVALID_TEXTURE_HANDLE);
        CurTextureTargets[unit] = texture->target;
    }
    glBindTexture(texture->target, texture->handle);
}

static void FreeTexture( Texture* texture )
{
    FreeReferenceCounter(&texture->refCounter);
    glDeleteTextures(1, &texture->handle);
    delete texture;
}

void ReferenceTexture( Texture* texture )
{
    Reference(&texture->refCounter);
}

void ReleaseTexture( Texture* texture )
{
    Release(&texture->refCounter);
    if(!HasReferences(&texture->refCounter))
        FreeTexture(texture);
}
