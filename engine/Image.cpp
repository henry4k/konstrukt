#include <assert.h>
#include <string.h> // memset

#include "Warnings.h"
BEGIN_EXTERNAL_CODE
#include <konstrukt_stb_image.h>
#include <konstrukt_stb_image_resize.h>
END_EXTERNAL_CODE

#include "Common.h"
#include "Vfs.h"
#include "OpenGL.h"
#include "Image.h"


void CreateImage( Image* image, int width, int height, int channelCount )
{
    memset(image, 0, sizeof(Image));

    image->width = width;
    image->height = height;
    image->channelCount = channelCount;

    image->type = GL_UNSIGNED_BYTE;

    image->data = Alloc(width*height*channelCount);
}

static int VfsRead( void* user, char* data, int size )
{
    VfsFile* file = (VfsFile*)user;
    return ReadVfsFile(file, data, size);
}

static void VfsSkip( void* user, int offset )
{
    VfsFile* file = (VfsFile*)user;
    SetVfsFilePos(file, GetVfsFilePos(file)+offset);
}

static int VfsEOF( void* user )
{
    VfsFile* file = (VfsFile*)user;
    return HasVfsFileEnded(file);
}

static const stbi_io_callbacks VfsCallbacks =
{
   VfsRead,
   VfsSkip,
   VfsEOF
};

void LoadImage( Image* image, const char* vfsPath )
{
    memset(image, 0, sizeof(Image));

    VfsFile* file = OpenVfsFile(vfsPath, VFS_OPEN_READ);

    stbi_set_flip_vertically_on_load(1);

    stbi_malloc = Alloc;
    stbi_realloc = ReAlloc;
    stbi_free = Free;
    image->data = stbi_load_from_callbacks(&VfsCallbacks,
                                           file,
                                           &image->width,
                                           &image->height,
                                           &image->channelCount,
                                           STBI_default);
    CloseVfsFile(file);
    if(!image->data)
        FatalError("Can't load '%s': %s", vfsPath, stbi_failure_reason());

    image->type = GL_UNSIGNED_BYTE;
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

void CreateResizedImage( Image* output,
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

    output->data = Alloc(width*height*channelCount);

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
    if(!stbir_resize_uint8_generic( inputPixels,  input->width,  input->height, 0,
                                   outputPixels, output->width, output->height, 0,
                                   channelCount, alphaChannel, flags,
                                   STBIR_EDGE_CLAMP,
                                   STBIR_FILTER_DEFAULT,
                                   STBIR_COLORSPACE_LINEAR,
                                   NULL))
        FatalError("Failed to resize image.");
}

void FreeImage( const Image* image )
{
    if(image->data)
        Free(image->data);
}
