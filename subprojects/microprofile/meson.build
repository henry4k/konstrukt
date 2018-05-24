project('microprofile', 'cpp')

config_data = configuration_data()
#config_data.set('has_little_endian', host_machine.endian() == 'little')

header = configure_file(input: 'microprofile.h.in',
                        output: 'microprofile.h',
                        configuration: config_data)

dependencies = [dependency('threads'),
                subproject('glad').get_variable('glad_dep')]

compiler = meson.get_compiler('cpp')
host_system = host_machine.system()
if host_system == 'windows'
    dependencies += [compiler.find_library('wsock32'),
                     compiler.find_library('ws2_32')]
endif

microprofile_dep = declare_dependency(
    link_with: static_library('microprofile', 'microprofile.cpp', header,
                              dependencies: dependencies),
    include_directories: include_directories('.'),
    dependencies: dependencies)

if host_system == 'windows'
    if compiler.sizeof('void *') == 8 # 64 bit architecture
        helper_arch = 'x64'
    else
        helper_arch = 'x86'
    endif
    install_data('source/bin/microprofile-win32-cswitch_'+helper_arch+'.exe',
                 'source/bin/microprofile-win32-cswitch_'+helper_arch+'.pdb',
                 install_dir: '.')
endif
