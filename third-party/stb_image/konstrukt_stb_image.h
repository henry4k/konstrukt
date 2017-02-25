#ifndef __KONSTRUKT_STB_IMAGE__
#define __KONSTRUKT_STB_IMAGE__

#include <stddef.h> // size_t

extern void* (*stbi_malloc)(size_t size);
extern void* (*stbi_realloc)(void* ptr, size_t size);
extern void  (*stbi_free)(void* ptr);
#define STBI_MALLOC(sz) stbi_malloc(sz)
#define STBI_REALLOC(p,sz) stbi_realloc(p,sz)
#define STBI_FREE(p) stbi_free(p)

#define STBI_NO_STDIO
#define STBI_NO_HDR
#define STBI_NO_LINEAR
#define STBI_ONLY_PNG

#include "stb_image.h"

#endif

