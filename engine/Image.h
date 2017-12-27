#ifndef __KONSTRUKT_IMAGE__
#define __KONSTRUKT_IMAGE__

#include "JobManager.h" // JobId, JobManager


struct Image
{
    int width, height;
    int channelCount;
    int type;
    void* data;
};

void CreateImage( Image* image, int width, int height, int channelCount );
void LoadImage( Image* image, const char* vfsPath );
void MultiplyImageRgbByAlpha( Image* image );
void CreateResizedImage( Image* output, const Image* input, int width, int height );
void FreeImage( const Image* image );

JobId LoadImageAsync( JobManager* jobManager, Image* image, const char* vfsPath );

#endif
