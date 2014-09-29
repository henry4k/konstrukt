#include <string.h> // memset

#define STBI_NO_HDR
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Common.h"
#include "OpenGL.h"
#include "Image.h"


bool CreateImage( Image* image, int width, int height, int bpp )
{
    memset(image, 0, sizeof(Image));

    image->width = width;
    image->height = height;
    image->bpp = bpp;

    switch(image->bpp)
    {
        case 1:
            image->format = GL_LUMINANCE;
            break;

        case 2:
            image->format = GL_LUMINANCE_ALPHA;
            break;

        case 3:
            image->format = GL_RGB;
            break;

        case 4:
            image->format = GL_RGBA;
            break;

        default:
            Error("Can't create image (Unknown BPP -> %d)", image->bpp);
            return false;
    }

    image->type = GL_UNSIGNED_BYTE;

    image->data = new char[width*height*bpp];

    return true;
}

bool LoadImage( Image* image, const char* file )
{
    memset(image, 0, sizeof(Image));

    image->data = (char*)stbi_load(file,
                                   &image->width,
                                   &image->height,
                                   &image->bpp,
                                   STBI_default);

    if(image->data)
    {
        switch(image->bpp)
        {
            case 1:
                image->format = GL_LUMINANCE;
                break;

            case 2:
                image->format = GL_LUMINANCE_ALPHA;
                break;

            case 3:
                image->format = GL_RGB;
                break;

            case 4:
                image->format = GL_RGBA;
                break;

            default:
                Error("Can't load '%s': Unknown BPP -> %d", file, image->bpp);
                delete[] image->data;
                return false;
        }

        image->type = GL_UNSIGNED_BYTE;

        return true;
    }
    else
    {
        Error("Can't load '%s': %s", file, stbi_failure_reason());
        return false;
    }
}


void FreeImage( const Image* image )
{
    if(image->data)
        delete[] image->data;
}
