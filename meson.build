project('konstrukt', 'cpp',
        version: '0.0.0',
        license: 'MIT',
        meson_version: '>0.46')

if get_option('use_relative_paths_in_debug_symbols')
    debug_prefix_map = '-fdebug-prefix-map='+meson.current_source_dir()+'/='
    if meson.get_compiler('cpp').has_argument(debug_prefix_map)
        add_global_arguments(debug_prefix_map, language: ['c', 'cpp'])
    endif
    # MSVC should do this by default.
    # If this is not the case, try to remove the /FC flag.
    # See https://msdn.microsoft.com/en-us/library/027c4t2s.aspx
endif

subdir('engine')

konstrukt_executable = executable('konstrukt',
                                  'engine/ExecutableMain.cpp',
                                  dependencies: konstrukt_dep,
                                  gui_app: true,
                                  install: true)

package_data = configuration_data()
package_data.set('version', meson.project_version())
package_data.set('system', host_machine.system())
package_data.set('architecture', host_machine.cpu_family()) # TODO: May be too unspecific.
konstrukt_basename = konstrukt_executable.full_path().split('/')[-1].split('\\')[-1]
package_data.set('executable', konstrukt_basename)
configure_file(input: 'package.json.in',
               output: 'package.json',
               configuration: package_data)
