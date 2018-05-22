project('stb', 'c')

include_dir = include_directories('.')

image_dep = declare_dependency(
    link_with: static_library('stb_image', 'konstrukt_stb_image.c'),
    include_directories: include_dir)

image_resize_dep = declare_dependency(
    link_with: static_library('stb_image_resize', 'konstrukt_stb_image_resize.c'),
    include_directories: include_dir)

sprintf_dep = declare_dependency(
    link_with: static_library('stb_sprintf', 'konstrukt_stb_sprintf.c'),
    include_directories: include_dir)
