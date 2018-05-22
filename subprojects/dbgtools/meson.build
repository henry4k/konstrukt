project('dbgtools', 'cpp')

include_dir = include_directories('source/include')

compiler = meson.get_compiler('cpp')

callstack_deps = []
callstack_link_args = []
if compiler.get_id() == 'msvc'
    callstack_deps += compiler.find_library('dbghelp')
elif compiler.has_link_argument('-rdynamic')
    callstack_link_args += '-rdynamic'
endif
callstack_dep = declare_dependency(
    link_with: static_library('dbgtools',
                              'source/src/callstack.cpp',
                               include_directories: include_dir),
    include_directories: include_dir,
    link_args: callstack_link_args,
    dependencies: callstack_deps)

install_data('source/LICENSE',
             install_dir: 'licenses',
             rename: meson.project_name()+'.txt')
