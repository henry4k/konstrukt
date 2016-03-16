#include <assert.h>
#include <stdlib.h> // malloc, free
#include <string.h> // memset

#include "Warnings.h"
BEGIN_EXTERNAL_CODE
#include <konstrukt_stb_image.h>
#include <konstrukt_stb_image_resize.h>
END_EXTERNAL_CODE

#include "Common.h"
#include "PhysFS.h"
#include "OpenGL.h"
#include "Image.h"


bool CreateImage( Image* image, int width, int height, int channelCount )
{
    memset(image, 0, sizeof(Image));

    image->width = width;
    image->height = height;
    image->channelCount = channelCount;

    image->type = GL_UNSIGNED_BYTE;

    image->data = malloc(width*height*channelCount);

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

    image->data = stbi_load_from_callbacks(&PhysFSCallbacks,
                                           file,
                                           &image->width,
                                           &image->height,
                                           &image->channelCount,
                                           STBI_default);
    PHYSFS_close(file);
    if(!image->data)
    {
        Error("Can't load '%s': %s", vfsPath, stbi_failure_reason());
        return false;
    }

    image->type = GL_UNSIGNED_BYTE;

    return true;
}

void MultiplyImageRgbByAlpha( Image* image )
{
    assert(image->type == GL_UNSIGNED_BYTE);

    unsigned char* data = (unsigned char*)image->data;
    const int pixelCount = image->width * image->height;

    switch(image->channelCount)
    {
        case 2: // luminance + alpha
            for(int i = 0; i < pixelCount; i++)
            {
                unsigned char* pixel = &data[i*2];
                const float alpha = (float)pixel[1] / 255.f;
                assert(alpha >= 0.f && alpha <= 1.f);
                pixel[0] *= alpha;
            }
            break;

        case 4: // rgb + alpha
            for(int i = 0; i < pixelCount; i++)
            {
                unsigned char* pixel = &data[i*4];
                const float alpha = (float)pixel[3] / 255.f;
                assert(alpha >= 0.f && alpha <= 1.f);
                pixel[0] *= alpha;
                pixel[1] *= alpha;
                pixel[2] *= alpha;
            }
            break;
    }
}

bool CreateResizedImage( Image* output,
                         const Image* input,
                         int width,
                         int height )
{
    assert(input->type == GL_UNSIGNED_BYTE);

    memset(output, 0, sizeof(Image));

    const int channelCount = input->channelCount;

    output->width  = width;
    output->height = height;
    output->channelCount = channelCount;
    output->type   = input->type;

    output->data = malloc(width*height*channelCount);

    const int flags = STBIR_FLAG_ALPHA_PREMULTIPLIED;
    // ^- Always assume premultiplied color channels and don't treat the alpha
    // channel specially.

    int alphaChannel;
    switch(channelCount)
    {
        case 2:
            alphaChannel = 1;
            break;

        case 4:
            alphaChannel = 3;
            break;

        default:
            alphaChannel = STBIR_ALPHA_CHANNEL_NONE;
    }

    const unsigned char* inputPixels  = (const unsigned char*)input->data;
          unsigned char* outputPixels =       (unsigned char*)output->data;
    if(stbir_resize_uint8_generic( inputPixels,  input->width,  input->height, 0,
                                  outputPixels, output->width, output->height, 0,
                                  channelCount, alphaChannel, flags,
                                  STBIR_EDGE_CLAMP,
                                  STBIR_FILTER_DEFAULT,
                                  STBIR_COLORSPACE_LINEAR,
                                  NULL))
    {
        return true;
    }
    else
    {
        Error("Failed to resize image.");
        return false;
    }
}

void FreeImage( const Image* image )
{
    if(image->data)
        free(image->data);
}
