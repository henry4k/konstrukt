project('Remotery', 'c')

config_data = configuration_data()
config_data.set('has_little_endian', host_machine.endian() == 'little')

header = configure_file(input: 'Remotery.h.in',
                        output: 'Remotery.h',
                        configuration: config_data)

dependencies = [dependency('threads')]

compiler = meson.get_compiler('c')
host_system = host_machine.system()
if host_system == 'windows'
    dependencies += [compiler.find_library('wsock32'),
                     compiler.find_library('ws2_32')]
else
    dependencies += compiler.find_library('dl')
endif

remotery_dep = declare_dependency(
    link_with: static_library('Remotery', 'Remotery.c', header),
    include_directories: include_directories('.'),
    dependencies: dependencies)

install_subdir('source/vis',
               install_dir: '.')

install_data('source/LICENSE',
             install_dir: 'licenses',
             rename: meson.project_name()+'.txt')
