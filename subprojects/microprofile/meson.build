project('microprofile', 'cpp')

dependencies = [dependency('threads'),
                subproject('glad').get_variable('glad_dep')] # TODO: Needed?

compiler = meson.get_compiler('cpp')
host_system = host_machine.system()
if host_system == 'windows'
    dependencies += [compiler.find_library('wsock32'),
                     compiler.find_library('ws2_32')]
endif

microprofile_dep = declare_dependency(
    link_with: static_library('microprofile', 'source/microprofile.cpp'),
    include_directories: include_directories('source'),
    dependencies: dependencies)

if compiler.sizeof('void *') == 8 # 64 bit architecture
    helper_arch = 'x64'
else
    helper_arch = 'x86'
endif
install_data('source/bin/microprofile-win32-cswitch_'+helper_arch+'.exe',
             'source/bin/microprofile-win32-cswitch_'+helper_arch+'.pdb',
             install_dir: '.')
