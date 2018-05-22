project('tinycthread', 'c')

dependencies = [dependency('threads')]

tinycthread_dep = declare_dependency(
    link_with: static_library('tinycthread', 'source/source/tinycthread.c'),
    include_directories: include_directories('source/source'),
    dependencies: dependencies)

install_data('LICENSE.txt',
             install_dir: 'licenses',
             rename: meson.project_name()+'.txt')
