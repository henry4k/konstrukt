#include <assert.h>
#include <stdlib.h> // malloc, free
#include <string.h> // memset

#include <apoapsis_stb_image.h>

#include "Common.h"
#include "PhysFS.h"
#include "OpenGL.h"
#include "Image.h"


bool CreateImage( Image* image, int width, int height, int bpp )
{
    memset(image, 0, sizeof(Image));

    image->width = width;
    image->height = height;
    image->bpp = bpp;

    switch(image->bpp)
    {
        case 1:
            image->format = GL_LUMINANCE;
            break;

        case 2:
            image->format = GL_LUMINANCE_ALPHA;
            break;

        case 3:
            image->format = GL_RGB;
            break;

        case 4:
            image->format = GL_RGBA;
            break;

        default:
            Error("Can't create image (Unknown BPP -> %d)", image->bpp);
            return false;
    }

    image->type = GL_UNSIGNED_BYTE;

    image->data = (char*)malloc(width*height*bpp);

    return true;
}

static int PhysFSRead( void* user, char* data, int size )
{
    PHYSFS_File* file = (PHYSFS_File*)user;
    return (int)PHYSFS_readBytes(file, data, size);
}

static void PhysFSSkip( void* user, int offset )
{
    PHYSFS_File* file = (PHYSFS_File*)user;
    const int currentPosition = PHYSFS_tell(file);
    assert(currentPosition >= 0);
    const int newPosition = currentPosition + offset;
    const int success = PHYSFS_seek(file, newPosition);
    assert(success);
}

static int PhysFSEOF( void* user )
{
    PHYSFS_File* file = (PHYSFS_File*)user;
    return PHYSFS_eof(file);
}

static const stbi_io_callbacks PhysFSCallbacks =
{
   PhysFSRead,
   PhysFSSkip,
   PhysFSEOF
};

bool LoadImage( Image* image, const char* vfsPath )
{
    memset(image, 0, sizeof(Image));

    PHYSFS_File* file = PHYSFS_openRead(vfsPath);
    if(!file)
    {
        Error("Can't load '%s': %s", vfsPath, PHYSFS_getLastError());
        return false;
    }

    stbi_set_flip_vertically_on_load(1);

    image->data = (char*)stbi_load_from_callbacks(&PhysFSCallbacks,
                                                  file,
                                                  &image->width,
                                                  &image->height,
                                                  &image->bpp,
                                                  STBI_default);
    PHYSFS_close(file);
    if(!image->data)
    {
        Error("Can't load '%s': %s", vfsPath, stbi_failure_reason());
        return false;
    }

    switch(image->bpp)
    {
        case 1:
            image->format = GL_LUMINANCE;
            break;

        case 2:
            image->format = GL_LUMINANCE_ALPHA;
            break;

        case 3:
            image->format = GL_RGB;
            break;

        case 4:
            image->format = GL_RGBA;
            break;

        default:
            Error("Can't load '%s': Unknown BPP -> %d", vfsPath, image->bpp);
            free(image->data);
            return false;
    }

    image->type = GL_UNSIGNED_BYTE;

    return true;
}

void FreeImage( const Image* image )
{
    if(image->data)
        free(image->data);
}
