project('lua-taggedcoro', 'c')

sources = ['source/src/taggedcoro.c',
           'source/src/isyieldable.c']

c_args = []

dependencies = [dependency(get_option('lua_package'),
                           version: get_option('lua_version').split(','))]

lua_taggedcoro_lib = static_library('lua-taggedcoro',
                                    sources,
                                    c_args: c_args,
                                    dependencies: dependencies)

lua_taggedcoro_dep = declare_dependency(
    link_with: lua_taggedcoro_lib,
    include_directories: include_directories('.'))

#install_data('source/LICENSE',
#             install_dir: 'licenses',
#             rename: meson.project_name()+'.txt')
