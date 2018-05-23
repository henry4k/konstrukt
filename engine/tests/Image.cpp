#include <assert.h>
#include "../Common.h" // REPEAT
#include "../Vfs.h"
#include "../Image.h"
#include "TestTools.h"
#include <dummy/inline.hpp>

#define InlineTest DUMMY_INLINE_TEST

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

static unsigned char GetPixelChannel( const Image* image,
                                      int x, int y,
                                      int channel )
{
    const int width        = GetImageWidth(image);
    const int height       = GetImageHeight(image);
    const int channelCount = GetImageChannelCount(image);
    const void* pixels     = GetImagePixels(image);

    assert(x >= 0 && x < width);
    assert(y >= 0 && y < height);
    assert(channel >= 0 && channel < channelCount);

    const int index = channelCount*width*y +
                      channelCount*x +
                      channel;

    return ((unsigned char*)pixels)[index];
}

InlineTest("load RGB image", dummySignalSandbox)
{
    Image image;
    LoadImage(&image, "data/Image/rgb.png");

    Require(image.width  == 32);
    Require(image.height == 32);
    Require(image.channelCount == 3);
    Require(image.data != NULL);

    Require(GetPixel(&image, 0,  0) == 0xFF0000);
    Require(GetPixel(&image, 15, 0) == 0x00FF00);
    Require(GetPixel(&image, 31, 0) == 0x0000FF);

    FreeImage(&image);
}

InlineTest("load RGBA image", dummySignalSandbox)
{
    Image image;
    LoadImage(&image, "data/Image/rgb_alpha.png");

    Require(image.width  == 32);
    Require(image.height == 32);
    Require(image.channelCount == 4);
    Require(image.data != NULL);

    Require(GetPixel(&image, 0,   0) == 0xFF0000FF);
    Require(GetPixel(&image, 15,  0) == 0x00FF00FF);
    Require(GetPixel(&image, 31,  0) == 0x0000FFFF);

    Require(GetPixel(&image, 0,  31) == 0xFF000080);
    Require(GetPixel(&image, 15, 31) == 0x00FF0080);
    Require(GetPixel(&image, 31, 31) == 0x0000FF80);

    FreeImage(&image);
}

InlineTest("load greyscale image", dummySignalSandbox)
{
    Image image;
    LoadImage(&image, "data/Image/greyscale.png");

    Require(image.width  == 32);
    Require(image.height == 32);
    Require(image.channelCount == 1);
    Require(image.data != NULL);

    Require(GetPixel(&image,  0, 0) == 0x00);
    Require(GetPixel(&image, 31, 0) == 0xFF);

    FreeImage(&image);
}

InlineTest("load greyscale alpha image", dummySignalSandbox)
{
    Image image;
    LoadImage(&image, "data/Image/greyscale_alpha.png");

    Require(image.width  == 32);
    Require(image.height == 32);
    Require(image.channelCount == 2);
    Require(image.data != NULL);

    Require(GetPixel(&image,  0,  0) == 0x00FF);
    Require(GetPixel(&image, 31,  0) == 0xFFFF);

    Require(GetPixel(&image,  0, 31) == 0x0080);
    Require(GetPixel(&image, 31, 31) == 0xFF80);

    FreeImage(&image);
}

InlineTest("load indexed rgb image", dummySignalSandbox)
{
    Image image;
    LoadImage(&image, "data/Image/indexed.png");

    Require(image.width  == 32);
    Require(image.height == 32);
    Require(image.channelCount == 3);
    Require(image.data != NULL);

    Require(GetPixel(&image,  0, 0) == 0x000000);
    Require(GetPixel(&image, 31, 0) == 0xFFFFFF);

    FreeImage(&image);
}

InlineTest("load indexed rgb image", dummySignalSandbox)
{
    Image image;
    LoadImage(&image, "data/Image/indexed.png");

    Require(image.width  == 32);
    Require(image.height == 32);
    Require(image.channelCount == 3);
    Require(image.data != NULL);

    Require(GetPixel(&image,  0, 0) == 0x000000);
    Require(GetPixel(&image, 31, 0) == 0xFFFFFF);

    FreeImage(&image);
}

InlineTest("MultiplyImageRgbByAlpha", dummySignalSandbox)
{
    Image image;
    LoadImage(&image, "data/Image/gradient.png");

    Require(image.width  == 32);
    Require(image.height == 32);
    Require(image.channelCount == 4);
    Require(image.data != NULL);

    Require(GetPixel(&image,  0, 0) == 0xFFFFFFFF);
    Require(GetPixel(&image, 31, 0) == 0xFFFFFF00);

    MultiplyImageRgbByAlpha(&image);

    Require(GetPixel(&image,  0, 0) == 0xFFFFFFFF);
    Require(GetPixel(&image, 31, 0) == 0x00000000);

    FreeImage(&image);
}

InlineTest("CreateResizedImage", dummySignalSandbox)
{
    Image image;
    LoadImage(&image, "data/Image/checker.png");

    Require(image.width  == 32);
    Require(image.height == 32);
    Require(image.channelCount == 3);
    Require(image.data != NULL);

    Require(GetPixel(&image, 0, 0) == 0xFFFFFF);
    Require(GetPixel(&image, 1, 0) == 0x000000);

    Image resizedImage;
    CreateResizedImage(&resizedImage, &image, 16, 16);

    Require(resizedImage.width  == 16);
    Require(resizedImage.height == 16);
    Require(resizedImage.channelCount == 3);
    Require(resizedImage.data != NULL);

    Require(GetPixel(&resizedImage,  0,  0) == 0x878787);
    Require(GetPixel(&resizedImage, 15, 15) == 0x878787);

    FreeImage(&image);
    FreeImage(&resizedImage);
}

#include "../JobManager.h"

InlineTest("load image asynchronously", dummySignalSandbox)
{
    JobManagerConfig managerConfig;
    managerConfig.workerThreads = 3;
    InitJobManager(managerConfig);

    Image image;
    const JobId job = LoadImageAsync(&image, "data/Image/rgb.png");

    LogNotice("loading ...");
    WaitForJobs(&job, 1);
    RemoveJob(job);
    LogNotice("loaded");

    DestroyJobManager();

    Require(image.width  == 32);
    Require(image.height == 32);
    Require(image.channelCount == 3);
    Require(image.data != NULL);

    Require(GetPixel(&image, 0,  0) == 0xFF0000);
    Require(GetPixel(&image, 15, 0) == 0x00FF00);
    Require(GetPixel(&image, 31, 0) == 0x0000FF);

    FreeImage(&image);
}

int main( int argc, char** argv )
{
    InitTests(argc, argv);
    InitTestVfs(argv[0]);
    MountVfsDir("data", "data", false);
    dummyAddInlineTests();
    return RunTests();
}
