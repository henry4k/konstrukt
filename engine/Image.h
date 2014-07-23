#ifndef __APOAPSIS_IMAGE__
#define __APOAPSIS_IMAGE__

struct Image
{
    int width, height;
    int bpp;
    int format, type;
    char* data;
};

bool CreateImage( Image* image, int width, int height, int bpp );
bool LoadImage( Image* image, const char* file );
void FreeImage( const Image* image );

#endif
