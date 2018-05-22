project('lua-cjson', 'c')

sources = ['source/lua_cjson.c',
           'source/strbuf.c']

c_args = []

dependencies = [dependency(get_option('lua_package'),
                           version: get_option('lua_version').split(','))]

if get_option('internal_fpconv')
    sources += 'source/dtoa.c'
    sources += 'source/g_fmt.c'
    c_args += '-DUSE_INTERNAL_FPCONV'

    if host_machine.endian() == 'big'
        c_args += '-DIEEE_BIG_ENDIAN'
    endif

    if get_option('multiple_threads')
        dependencies += dependency('threads')
        c_args += '-DMULTIPLE_THREADS'
    endif
else
    sources += 'source/fpconv.c'
endif

compiler = meson.get_compiler('c')
if compiler.has_header_symbol('math.h', 'isinf')
    c_args += '-DUSE_INTERNAL_ISINF'
endif

if host_machine.system() == 'windows'
    # Windows sprintf()/strtod() handle NaN/inf differently. Not supported.
    c_args += '-DDISABLE_INVALID_NUMBERS'
endif

lua_cjson_lib = static_library('lua-cjson',
                               sources,
                               c_args: c_args,
                               dependencies: dependencies)

lua_cjson_dep = declare_dependency(
    link_with: lua_cjson_lib,
    include_directories: include_directories('.'))

install_data('source/LICENSE',
             install_dir: 'licenses',
             rename: meson.project_name()+'.txt')
