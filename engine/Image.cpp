#include <assert.h>
#include <string.h> // memset

#include "Warnings.h"
BEGIN_EXTERNAL_CODE
#include <konstrukt_stb_image.h>
#include <konstrukt_stb_image_resize.h>
END_EXTERNAL_CODE

#include "Common.h"
#include "Vfs.h"
#include "FsUtils.h" // MAX_PATH_SIZE
#include "JobManager.h" //
#include "OpenGL.h" // GL_UNSIGNED_BYTE
#include "Reference.h"
#include "Image.h"

struct Image
{
    ReferenceCounter refCounter;
    int width, height;
    int channelCount;
    int type;
    void* data;
};

static Image* CreateImage()
{
    Image* image = NEW(Image);
    InitReferenceCounter(&image->refCounter);
    return image;
}

static void FreeImage( Image* image )
{
    if(image->data)
        Free(image->data);
    DELETE(image);
}

void ReferenceImage( Image* image )
{
    Reference(&image->refCounter);
}

void ReleaseImage( Image* image )
{
    Release(&image->refCounter);
    if(!HasReferences(&image->refCounter))
        FreeImage(image);
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

static Image* LoadImage( const char* vfsPath )
{
    Image* image = CreateImage();

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

    return image;
}

static void MultiplyImageRgbByAlpha( Image* image )
{
    assert(image->type == GL_UNSIGNED_BYTE);

    unsigned char* data = (unsigned char*)image->data;
    const int pixelCount = image->width * image->height;

    switch(image->channelCount)
    {
        case 2: // luminance + alpha
            REPEAT(pixelCount, i)
            {
                unsigned char* pixel = &data[i*2];
                const float alpha = (float)pixel[1] / 255.f;
                assert(alpha >= 0.f && alpha <= 1.f);
                pixel[0] *= alpha;
            }
            break;

        case 4: // rgb + alpha
            REPEAT(pixelCount, i)
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

static Image* CreateResizedImage( const Image* input,
                                  int width,
                                  int height )
{
    assert(input->type == GL_UNSIGNED_BYTE);

    Image* output = CreateImage();

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

    return output;
}

// ---------------------------------------------------------------------------

enum ImageCreationJobType
{
    LOAD_IMAGE_JOB,
    RESIZE_IMAGE_JOB
};

struct ImageCreationJobDesc
{
    Image* result;
    ImageCreationJobType type;
    union
    {
        struct
        {
            char vfsPath[MAX_PATH_SIZE];
        } loadImage;

        struct
        {
            Image* input;
            int width;
            int height;
        } resizeImage;
    } params;
};

static void DestroyImageCreationJob( void* _desc )
{
    ImageCreationJobDesc* desc = (ImageCreationJobDesc*)_desc;

    if(desc->result)
        ReleaseImage(desc->result);

    switch(desc->type)
    {
        case LOAD_IMAGE_JOB:
            break;

        case RESIZE_IMAGE_JOB:
            ReleaseImage(desc->params.resizeImage.input);
            break;
    }

    DELETE(desc);
}

static void ProcessImageCreationJob( void* _desc )
{
    ImageCreationJobDesc* desc = (ImageCreationJobDesc*)_desc;
    assert(desc->result == NULL);
    switch(desc->type)
    {
        case LOAD_IMAGE_JOB:
            desc->result = LoadImage(desc->params.loadImage.vfsPath);
            break;

        case RESIZE_IMAGE_JOB:
            desc->result = CreateResizedImage(desc->params.resizeImage.input,
                                              desc->params.resizeImage.width,
                                              desc->params.resizeImage.height);
            break;
    }
    ReferenceImage(desc->result);
}

JobId BeginLoadingImage( const char* vfsPath )
{
    ImageCreationJobDesc* desc = NEW(ImageCreationJobDesc);
    desc->result = NULL;
    desc->type = LOAD_IMAGE_JOB;
    CopyString(vfsPath, desc->params.loadImage.vfsPath, MAX_PATH_SIZE);
    return CreateJob({"LoadImage",
                      ProcessImageCreationJob,
                      DestroyImageCreationJob,
                      desc});
}

JobId BeginResizingImage( Image* input, int width, int height )
{
    ImageCreationJobDesc* desc = NEW(ImageCreationJobDesc);
    desc->result = NULL;
    desc->type = RESIZE_IMAGE_JOB;
    desc->params.resizeImage.input  = input;
    desc->params.resizeImage.width  = width;
    desc->params.resizeImage.height = height;
    ReferenceImage(input); // released in DestroyImageCreationJob
    return CreateJob({"ResizeImage",
                      ProcessImageCreationJob,
                      DestroyImageCreationJob,
                      desc});
}

Image* GetCreatedImage( JobId job )
{
    Ensure(GetJobStatus(job) == COMPLETED_JOB);
    ImageCreationJobDesc* desc = (ImageCreationJobDesc*)GetJobData(job);
    assert(desc->result);
    return desc->result;
}

// ---------------------------------------------------------------------------

static void DestroyImagePremultiplicationJob( void* _desc )
{
    Image* image = (Image*)_desc;
    ReleaseImage(image); // referenced in MultiplyImageRgbByAlpha_
}

static void ProcessImagePremultiplicationJob( void* _desc )
{
    Image* image = (Image*)_desc;
    MultiplyImageRgbByAlpha(image);
}

JobId MultiplyImageRgbByAlpha_( Image* image )
{
    ReferenceImage(image); // released in DestroyImagePremultiplicationJob
    return CreateJob({"MultiplyImageRgbByAlpha",
                      ProcessImagePremultiplicationJob,
                      DestroyImagePremultiplicationJob,
                      image});
}

// ---------------------------------------------------------------------------

int GetImageWidth( const Image* image )
{
    return image->width;
}

int GetImageHeight( const Image* image )
{
    return image->height;
}

int GetImageChannelCount( const Image* image )
{
    return image->channelCount;
}

int GetImageType( const Image* image )
{
    return image->type;
}

void* GetImagePixels( const Image* image )
{
    return image->data;
}
