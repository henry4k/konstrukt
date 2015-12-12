#include <stdlib.h> // NULL

#include "Common.h"
#include "OpenGL.h"
#include "Image.h"
#include "Reference.h"
#include "Texture.h"


static const GLuint INVALID_TEXTURE_HANDLE = 0;


struct Texture
{
    ReferenceCounter refCounter;
    GLenum target;
    GLuint handle;
};


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
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 image->format,
                 image->width,
                 image->height,
                 0,
                 image->format,
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
        const GLenum target = GL_TEXTURE_CUBE_MAP_POSITIVE_X+i;
        glTexImage2D(target,
                     0,
                     images[i]->format,
                     images[i]->width,
                     images[i]->height,
                     0,
                     images[i]->format,
                     images[i]->type,
                     images[i]->data);
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

static GLenum CurTextureTargets[MAX_TEXTURE_UNITS];

static int InitTextureUnits()
{
    for(int i = 0; i < MAX_TEXTURE_UNITS; i++)
        CurTextureTargets[i] = INVALID_TEXTURE_HANDLE;
    return 0;
}

void BindTexture( const Texture* texture, int unit )
{
    static int unused = InitTextureUnits();
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
