#ifndef __IMAGE__
#define __IMAGE__

struct Image
{
    int height, width;
    int bpp;
    int format, type;
    char* data;
};

bool LoadImage( Image* image, const char* file );
void FreeImage( const Image* image );

#endif
