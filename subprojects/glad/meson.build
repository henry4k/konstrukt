project('glad', 'c')

python = import('python3').find_python()

gl_version = '3.2'
gl_extensions = ['GL_ARB_debug_output',
                 'GL_EXT_texture_filter_anisotropic',
                 'GL_ARB_timer_query'] # used by MicroProfile

glad_script = files('source/glad/__main__.py')

sources = custom_target('generate-glad-sources',
    output: ['glad.h',
             'glad.c'],
    command: [python, glad_script,
                      '--api', 'gl='+gl_version,
                      '--profile', 'core',
                      '--spec', 'gl',
                      '--extensions', ','.join(gl_extensions),
                      '--no-loader',
                      '--generator', 'c',
                      '--omit-khrplatform',
                      '--local-files',
                      '--out-path', '@OUTDIR@'])

dependencies = []
compiler = meson.get_compiler('c')
host_system = host_machine.system()
if host_system != 'windows'
    dependencies += compiler.find_library('dl')
endif

glad_dep = declare_dependency(
    link_with: static_library('glad', sources),
    include_directories: include_directories('.'),
    dependencies: dependencies)
