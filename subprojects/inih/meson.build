project('inih', 'c')

inih_dep = declare_dependency(
    link_with: static_library('inih', 'source/ini.c'),
    include_directories: include_directories('source'))

install_data('source/LICENSE.txt',
             install_dir: 'licenses',
             rename: meson.project_name()+'.txt')
