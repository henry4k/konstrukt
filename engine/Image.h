#ifndef __KONSTRUKT_IMAGE__
#define __KONSTRUKT_IMAGE__

struct Image
{
    int width, height;
    int channelCount;
    int type;
    void* data;
};

bool CreateImage( Image* image, int width, int height, int channelCount );
bool LoadImage( Image* image, const char* vfsPath );
void MultiplyImageRgbByAlpha( Image* image );
bool CreateResizedImage( Image* output, const Image* input, int width, int height );
void FreeImage( const Image* image );

#endif
