project('dummy', 'cpp')

sources = ['source/dummy/core.c',
           'source/dummy/exception_sandbox.cpp',
           'source/dummy/inline.cpp',
           'source/dummy/signal.c',
           'source/dummy/signal_sandbox.c',
           'source/dummy/tap_reporter.c',
           'source/dummy/xml_reporter.c']

dummy_dep = declare_dependency(
    link_with: static_library('dummy', sources),
    include_directories: include_directories('source'))
