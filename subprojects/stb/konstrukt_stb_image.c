#define STB_IMAGE_IMPLEMENTATION
#include "konstrukt_stb_image.h"
void* (*stbi_malloc)(size_t size);
void* (*stbi_realloc)(void* ptr, size_t size);
void  (*stbi_free)(void* ptr);

