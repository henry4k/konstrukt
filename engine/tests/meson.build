test_deps = [konstrukt_dep,
             subproject('dummy').get_variable('dummy_dep'),
             declare_dependency(sources: ['TestTools.cpp'])]

workdir = meson.current_source_dir()

# Tests:
foreach name : ['Array',
                'Crc32',
                'FixedArray',
                'FsUtils',
                'Image',
                'ObjectSystem',
                'BitCondition',
                'Config',
                'Common',
                'Lua',
                'Math',
                'MeshBuffer',
                'PhysicsWorld',
                'Time',
                'Vfs',
                'JobManager']
    test(name,
         executable(name,
                    name+'.cpp',
                    dependencies: test_deps),
         workdir: workdir)
endforeach

# Benchmarks:
ObjectSystemPerformance = executable('ObjectSystemPerformance',
                                     'ObjectSystemPerformance.cpp',
                                     dependencies: test_deps)
benchmark('ObjectSystem',
          ObjectSystemPerformance,
          args: ['-Dtest.loop-count=5000'])

benchmark('JobManager',
          executable('JobManagerBenchmark',
                     'JobManagerBenchmark.cpp',
                     dependencies: test_deps))
