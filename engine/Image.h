#ifndef __KONSTRUKT_IMAGE__
#define __KONSTRUKT_IMAGE__

#include "JobManager.h" // JobId


struct Image;


void ReferenceImage( Image* image );

void ReleaseImage( Image* image );

/**
 * Starts a job which will load the given image file.
 *
 * Once the job is completed the image can be retrieved using
 * #GetCreatedImage.
 */
JobId BeginLoadingImage( const char* vfsPath );

/**
 * Starts a job which will resample the given image file.
 *
 * Once the job is completed the image can be retrieved using
 * #GetCreatedImage.
 */
JobId BeginResizingImage( Image* input, int width, int height );

Image* GetCreatedImage( JobId job );

/**
 * The RGB channels will be multiplied by the alpha channel.
 *
 * Note that the image *mustn't* be used while the job runs!
 *
 * @see https://developer.nvidia.com/content/alpha-blending-pre-or-not-pre
 * @see https://blogs.msdn.microsoft.com/shawnhar/2009/11/06/premultiplied-alpha
 */
JobId MultiplyImageRgbByAlpha_( Image* image ); // TODO: Name is temporary!

int GetImageWidth( const Image* image );
int GetImageHeight( const Image* image );
int GetImageChannelCount( const Image* image );
int GetImageType( const Image* image );
void* GetImagePixels( const Image* image );

#endif
