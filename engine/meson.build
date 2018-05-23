sources = ['AttachmentTarget.cpp',
           'Audio.cpp',
           'BitCondition.cpp',
           'Camera.cpp',
           'Common.cpp',
           'Config.cpp',
           'Controls.cpp',
           'Crc32.cpp',
           #'ExecutableMain.cpp', # May not be part of the library.
           'FsUtils.cpp',
           'Image.cpp',
           'JobManager.cpp',
           'LightWorld.cpp',
           'Lua.cpp',
           'Main.cpp',
           'Math.cpp',
           'MeshBuffer.cpp',
           'MeshChunkGenerator.cpp',
           'Mesh.cpp',
           'ModelWorld.cpp',
           'PhysicsWorld.cpp',
           'Reference.cpp',
           'RenderManager.cpp',
           'RenderTarget.cpp',
           'Shader.cpp',
           'SimulationGroup.cpp',
           'Texture.cpp',
           'Time.cpp',
           'Vertex.cpp',
           'Vfs.cpp',
           'VoxelVolume.cpp',
           'Window.cpp',
           'control_bindings/Joystick.cpp',
           'control_bindings/Keyboard.cpp',
           'control_bindings/Mouse.cpp',
           'lua_bindings/AttachmentTarget.cpp',
           'lua_bindings/Audio.cpp',
           'lua_bindings/Camera.cpp',
           'lua_bindings/Config.cpp',
           'lua_bindings/Controls.cpp',
           'lua_bindings/Image.cpp',
           'lua_bindings/LightWorld.cpp',
           'lua_bindings/Math.cpp',
           'lua_bindings/MeshBuffer.cpp',
           'lua_bindings/MeshChunkGenerator.cpp',
           'lua_bindings/Mesh.cpp',
           'lua_bindings/ModelWorld.cpp',
           'lua_bindings/PhysicsWorld.cpp',
           'lua_bindings/RenderManager.cpp',
           'lua_bindings/RenderTarget.cpp',
           'lua_bindings/Shader.cpp',
           'lua_bindings/Texture.cpp',
           'lua_bindings/Time.cpp',
           'lua_bindings/Vfs.cpp',
           'lua_bindings/VoxelVolume.cpp',
           'lua_bindings/JobManager.cpp',
           'vfs/PhysFS.cpp',
           'vfs/Real.cpp']

dependencies = [dependency('gl'), # TODO: As GLFW dynamically loads OpenGL this should be removed.
                dependency('glfw3', version: '>=3.2'),
                dependency('physfs', version: '>=3.0'),
                dependency('bullet', version: '>=2.87'),
                dependency('openal'),
                dependency('alure', version: '>=1.2'),
                dependency(get_option('lua_package'),
                           version: get_option('lua_version').split(',')),
                subproject('lua-cjson').get_variable('lua_cjson_dep'),
                subproject('glad').get_variable('glad_dep'),
                subproject('inih').get_variable('inih_dep'),
                subproject('tinycthread').get_variable('tinycthread_dep'),
                subproject('stb').get_variable('image_dep'),
                subproject('stb').get_variable('image_resize_dep'),
                subproject('stb').get_variable('sprintf_dep')]

override_options = []

constants_data = configuration_data()
constants_data.set('SIMULATION_FREQUENCY', 20)
constants_data.set('MAX_FRAME_FREQUENCY', 120)
constants_data.set_quoted('DEFAULT_PACKAGE_SEARCH_PATH', '.')
constants_data.set('KONSTRUKT_GL_DEBUG_FIX', get_option('gl_debug_fix'))
constants_data.set('KONSTRUKT_DEBUG_MESH', get_option('debug_mesh'))

if host_machine.system() == 'windows'
    compiler = meson.get_compiler('c')
    constants_data.set('OS_CALLING_CONVENTION',
                       compiler.get_define('APIENTRY',
                                           prefix: '#include <windows.h>'))
endif

if get_option('strict_compiler')
    override_options += ['warnlevel=3',
                         'werror=true']
endif

constants_data.set('KONSTRUKT_STACKTRACE_ENABLED', get_option('stacktrace'))
if get_option('stacktrace')
    dependencies += subproject('dbgtools').get_variable('dbgtools_dep')
endif

constants_data.set('KONSTRUKT_PROFILER_ENABLED', get_option('profiler'))
if get_option('profiler')
    sources += ['Profiler.cpp',
                'profiler/MicroProfile.cpp',
                'profiler/Remotery.cpp']
    dependencies += subproject('Remotery').get_variable('remotery_dep')
    dependencies += subproject('microprofile').get_variable('microprofile_dep')
endif

constants_file = configure_file(input: 'Constants.h.in',
                                output: 'Constants.h',
                                configuration: constants_data)

konstrukt_dep = declare_dependency(
    link_with: static_library('konstrukt',
                              sources,
                              constants_file,
                              override_options: override_options,
                              dependencies: dependencies),
    dependencies: dependencies)

subdir('tests')
