#ifndef __IMAGE__
#define __IMAGE__

struct Image
{
    int height, width;
    int bpp;
    int format, type;
    char* data;

    static bool Load( Image* image, const char* file );
    static void Free( const Image* image );
};

#endif
