#include <assert.h>
#include "../Common.h" // REPEAT
#include "../Vfs.h"
#include "../JobManager.h"
#include "../Image.h"
#include "TestTools.h"


static unsigned int GetPixel( const Image* image, int x, int y )
{
    const int width        = GetImageWidth(image);
    const int height       = GetImageHeight(image);
    const int channelCount = GetImageChannelCount(image);
    const void* pixels     = GetImagePixels(image);

    assert(x >= 0 && x < width);
    assert(y >= 0 && y < height);

    const int index = channelCount*width*y +
                      channelCount*x;

    unsigned char* pixel = (unsigned char*)pixels + index;

    unsigned int r = 0;
    REPEAT(channelCount, i)
        r = (r << 8) | pixel[i];
    return r;
}

static Image* LoadImage( const char* vfsPath )
{
    const JobId job = BeginLoadingImage(vfsPath);
    WaitForJobs(&job, 1);
    Image* image = GetCreatedImage(job);
    ReferenceImage(image);
    RemoveJob(job);
    return image;
}

InlineTest("load RGB image")
{
    Image* image = LoadImage("data/Image/rgb.png");

    Require(image != NULL);
    Require(GetImageWidth(image) == 32);
    Require(GetImageHeight(image) == 32);
    Require(GetImageChannelCount(image) == 3);
    Require(GetImagePixels(image) != NULL);

    Require(GetPixel(image, 0,  0) == 0xFF0000);
    Require(GetPixel(image, 15, 0) == 0x00FF00);
    Require(GetPixel(image, 31, 0) == 0x0000FF);

    ReleaseImage(image);
}

InlineTest("load RGBA image")
{
    Image* image = LoadImage("data/Image/rgb_alpha.png");

    Require(image != NULL);
    Require(GetImageWidth(image) == 32);
    Require(GetImageHeight(image) == 32);
    Require(GetImageChannelCount(image) == 4);
    Require(GetImagePixels(image) != NULL);

    Require(GetPixel(image, 0,   0) == 0xFF0000FF);
    Require(GetPixel(image, 15,  0) == 0x00FF00FF);
    Require(GetPixel(image, 31,  0) == 0x0000FFFF);

    Require(GetPixel(image, 0,  31) == 0xFF000080);
    Require(GetPixel(image, 15, 31) == 0x00FF0080);
    Require(GetPixel(image, 31, 31) == 0x0000FF80);

    ReleaseImage(image);
}

InlineTest("load greyscale image")
{
    Image* image = LoadImage("data/Image/greyscale.png");

    Require(image != NULL);
    Require(GetImageWidth(image) == 32);
    Require(GetImageHeight(image) == 32);
    Require(GetImageChannelCount(image) == 1);
    Require(GetImagePixels(image) != NULL);

    Require(GetPixel(image,  0, 0) == 0x00);
    Require(GetPixel(image, 31, 0) == 0xFF);

    ReleaseImage(image);
}

InlineTest("load greyscale alpha image")
{
    Image* image = LoadImage("data/Image/greyscale_alpha.png");

    Require(image != NULL);
    Require(GetImageWidth(image) == 32);
    Require(GetImageHeight(image) == 32);
    Require(GetImageChannelCount(image) == 2);
    Require(GetImagePixels(image) != NULL);

    Require(GetPixel(image,  0,  0) == 0x00FF);
    Require(GetPixel(image, 31,  0) == 0xFFFF);

    Require(GetPixel(image,  0, 31) == 0x0080);
    Require(GetPixel(image, 31, 31) == 0xFF80);

    ReleaseImage(image);
}

InlineTest("load indexed rgb image")
{
    Image* image = LoadImage("data/Image/indexed.png");

    Require(image != NULL);
    Require(GetImageWidth(image) == 32);
    Require(GetImageHeight(image) == 32);
    Require(GetImageChannelCount(image) == 3);
    Require(GetImagePixels(image) != NULL);

    Require(GetPixel(image,  0, 0) == 0x000000);
    Require(GetPixel(image, 31, 0) == 0xFFFFFF);

    ReleaseImage(image);
}

InlineTest("MultiplyImageRgbByAlpha")
{
    Image* image = LoadImage("data/Image/gradient.png");

    Require(image != NULL);
    Require(GetImageWidth(image) == 32);
    Require(GetImageHeight(image) == 32);
    Require(GetImageChannelCount(image) == 4);
    Require(GetImagePixels(image) != NULL);

    Require(GetPixel(image,  0, 0) == 0xFFFFFFFF);
    Require(GetPixel(image, 31, 0) == 0xFFFFFF00);

    const JobId job = MultiplyImageRgbByAlpha_(image);
    WaitForJobs(&job, 1);
    RemoveJob(job);

    Require(GetPixel(image,  0, 0) == 0xFFFFFFFF);
    Require(GetPixel(image, 31, 0) == 0x00000000);

    ReleaseImage(image);
}

InlineTest("CreateResizedImage")
{
    Image* image = LoadImage("data/Image/checker.png");

    Require(image != NULL);
    Require(GetImageWidth(image) == 32);
    Require(GetImageHeight(image) == 32);
    Require(GetImageChannelCount(image) == 3);
    Require(GetImagePixels(image) != NULL);

    Require(GetPixel(image, 0, 0) == 0xFFFFFF);
    Require(GetPixel(image, 1, 0) == 0x000000);

    const JobId job = BeginResizingImage(image, 16, 16);
    WaitForJobs(&job, 1);
    Image* resizedImage = GetCreatedImage(job);
    ReferenceImage(resizedImage);
    RemoveJob(job);

    Require(resizedImage != NULL);
    Require(GetImageWidth(resizedImage) == 16);
    Require(GetImageHeight(resizedImage) == 16);
    Require(GetImageChannelCount(resizedImage) == 3);
    Require(GetImagePixels(resizedImage) != NULL);

    Require(GetPixel(resizedImage,  0,  0) == 0x878787);
    Require(GetPixel(resizedImage, 15, 15) == 0x878787);

    ReleaseImage(image);
    ReleaseImage(resizedImage);
}


int main( int argc, char** argv )
{
    InitTests(argc, argv);
    InitTestVfs(argv[0]);
    InitTestJobManager();
    MountVfsDir("data", "data", false);
    return RunTests();
}
