#include <string.h>

#include "Common.h"
#include "OpenGL.h"
#include "Image.h"
#include "Texture.h"

bool Create2dTexture( Texture* texture, int options, const Image* image )
{
    Handle id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

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

    texture->name = id;
    texture->type = GL_TEXTURE_2D;
    return true;
}

bool Load2dTexture( Texture* texture, int options, const char* file )
{
    Image image;
    if(!LoadImage(&image, file))
        return false;
    bool r = Create2dTexture(texture, options, &image);
    FreeImage(&image);
    return r;
}

bool CreateCubeTexture( Texture* texture, int options, const Image* images )
{
    Handle id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);

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

    texture->name = id;
    texture->type = GL_TEXTURE_CUBE_MAP;
    return true;
}

bool LoadCubeTexture( Texture* texture, int options, const char* path, const char* extension )
{
    Image images[6];
    static const char* names[6] = { "Right","Left","Bottom","Top","Front","Back" };
    char buffer[512];
    strncpy(buffer, path, 512);
    int pos = strlen(path);
    for(int i = 0; i < 6; i++)
    {
        int namelen = strlen(names[i]);
        strncpy(buffer+pos, names[i], 512-pos);
        strncpy(buffer+pos+namelen, extension, 512-(pos+namelen));
        Log("%s", buffer);
        if(!LoadImage(&images[i], buffer))
            return false;
    }
    bool r = CreateCubeTexture(texture, options, images);
    for(int i = 0; i < 6; i++)
    {
        FreeImage(&images[i]);
    }
    return r;
}

void FreeTexture( const Texture* texture )
{
    glDeleteTextures(1, &texture->name);
}

static const int MaxTextureLayers = 8;
GLenum CurTextureTargets[MaxTextureLayers];

int InitTextureLayers()
{
    for(int i = 0; i < 8; i++)
        CurTextureTargets[i] = 0;
    return 0;
}

void BindTexture( const Texture* texture, int layer )
{
    static int unused = InitTextureLayers();
    glActiveTexture(GL_TEXTURE0+layer);
    if(CurTextureTargets[layer] && (CurTextureTargets[layer] != texture->type))
    {
        glBindTexture(CurTextureTargets[layer], 0);
        CurTextureTargets[layer] = texture->type;
    }
    glBindTexture(texture->type, texture->name);
}
